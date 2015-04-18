/*
 * Copyright (c) Aaron Gallagher <_@habnab.it>
 * See COPYING for details.
 */

#include <errno.h>
#include <string.h>
#include "passacre.h"
#include "keccak/KeccakSponge.h"
#include "skein/skeinApi.h"
#include "skein/threefishApi.h"
#include "scrypt/crypto_scrypt.h"

#define PASSACRE_SCRYPT_BUFFER_SIZE 64


enum passacre_gen_mode {
    PASSACRE_GEN_INVALID,
    PASSACRE_GEN_INITED,
    PASSACRE_GEN_KDF_SELECTED,
    PASSACRE_GEN_ABSORBED_PASSWORD,
    PASSACRE_GEN_ABSORBED_NULLS,
    PASSACRE_GEN_SQUEEZING,
};

/*
 * INITED -> ABSORBED_PASSWORD -> ABSORBED_NULLS -> SQUEEZING
 *   v            ^      v                             ^
 * KDF_SELECTED --'      '-----------------------------'
 */


enum passacre_gen_kdf {
    PASSACRE_NO_KDF,
    PASSACRE_SCRYPT,
};


struct passacre_gen_state {
    enum passacre_gen_mode mode;
    enum passacre_gen_algorithm algorithm;
    enum passacre_gen_kdf kdf;
    union {
        struct _scrypt_state {
            uint64_t N;
            uint32_t r;
            uint32_t p;
            unsigned char *persistence_buffer;
        } scrypt;
    } kdf_params;
    union {
        spongeState keccak;
        SkeinCtx_t skein;
        struct _skein_prng_state {
            ThreefishKey_t threefish;
            uint8_t buffer[64];
            unsigned char bytes_remaining;
        } skein_prng;
    } hasher;
};


size_t
passacre_gen_size(void)
{
    return sizeof (struct passacre_gen_state);
}


size_t
passacre_gen_scrypt_buffer_size(void)
{
    return PASSACRE_SCRYPT_BUFFER_SIZE;
}


int
passacre_gen_init(struct passacre_gen_state *state, enum passacre_gen_algorithm algo)
{
    uint8_t nulls[64] = {0};
    memset(state, 0, sizeof *state);
    switch (algo) {
    case PASSACRE_KECCAK:
        if (InitSponge(&state->hasher.keccak, 64, 1536)) {
            return -EINVAL;
        }
        break;

    case PASSACRE_SKEIN:
        if (skeinCtxPrepare(&state->hasher.skein, Skein512) != SKEIN_SUCCESS) {
            return -EINVAL;
        }
        if (skeinInit(&state->hasher.skein, 512) != SKEIN_SUCCESS) {
            return -EINVAL;
        }
        if (skeinUpdate(&state->hasher.skein, nulls, 64) != SKEIN_SUCCESS) {
            return -EINVAL;
        }
        break;

    default:
        return -EINVAL;
    }

    state->algorithm = algo;
    state->mode = PASSACRE_GEN_INITED;
    return 0;
}


int
passacre_gen_use_scrypt(
    struct passacre_gen_state *state,
    uint64_t N, uint32_t r, uint32_t p,
    unsigned char *persistence_buffer)
{
    struct _scrypt_state *s = &state->kdf_params.scrypt;
    if (state->mode != PASSACRE_GEN_INITED) {
        return -EINVAL;
    }
    state->kdf = PASSACRE_SCRYPT;
    s->N = N;
    s->r = r;
    s->p = p;
    s->persistence_buffer = persistence_buffer;
    if (s->persistence_buffer) {
        memset(s->persistence_buffer, 'x', PASSACRE_SCRYPT_BUFFER_SIZE);
    }
    state->mode = PASSACRE_GEN_KDF_SELECTED;
    return 0;
}


static int
passacre_gen_absorb(struct passacre_gen_state *state, const unsigned char *input, size_t n_bytes)
{
    switch (state->algorithm) {
    case PASSACRE_KECCAK:
        if (Absorb(&state->hasher.keccak, input, n_bytes * 8)) {
            return -EINVAL;
        }
        break;

    case PASSACRE_SKEIN:
        if (skeinUpdate(&state->hasher.skein, input, n_bytes) != SKEIN_SUCCESS) {
            return -EINVAL;
        }
        break;

    default:
        return -EINVAL;
    }

    return 0;
}


static const unsigned char PASSACRE_DELIMITER[1] = ":";


int
passacre_gen_absorb_username_password_site(
    struct passacre_gen_state *state,
    const unsigned char *username, size_t username_length,
    const unsigned char *password, size_t password_length,
    const unsigned char *site, size_t site_length)
{
    int result;
    switch (state->mode) {
    case PASSACRE_GEN_INITED:
    case PASSACRE_GEN_KDF_SELECTED:
        break;
    default:
        return -EINVAL;
    }
    if (state->kdf == PASSACRE_SCRYPT) {
        unsigned char outbuf[PASSACRE_SCRYPT_BUFFER_SIZE];
        struct _scrypt_state *s = &state->kdf_params.scrypt;
        if (crypto_scrypt(password, password_length,
                          username, username_length,
                          s->N, s->r, s->p,
                          outbuf, sizeof outbuf)) {
            return -errno;
        }
        if ((result = passacre_gen_absorb(state, outbuf, sizeof outbuf))) {
            return result;
        }
        if (s->persistence_buffer) {
            memcpy(s->persistence_buffer, outbuf, sizeof outbuf);
        }
    } else {
        if (username) {
            if ((result = passacre_gen_absorb(state, username, username_length))) {
                return result;
            }
            if ((result = passacre_gen_absorb(state, PASSACRE_DELIMITER, sizeof PASSACRE_DELIMITER))) {
                return result;
            }
        }
        if ((result = passacre_gen_absorb(state, password, password_length))) {
            return result;
        }
    }
    if ((result = passacre_gen_absorb(state, PASSACRE_DELIMITER, sizeof PASSACRE_DELIMITER))) {
        return result;
    }
    if ((result = passacre_gen_absorb(state, site, site_length))) {
        return result;
    }
    state->mode = PASSACRE_GEN_ABSORBED_PASSWORD;
    return 0;
}


int
passacre_gen_absorb_null_rounds(struct passacre_gen_state *state, size_t n_rounds)
{
    int result;
    unsigned char nulls[1024] = {0};
    size_t i;
    switch (state->mode) {
    case PASSACRE_GEN_ABSORBED_PASSWORD:
    case PASSACRE_GEN_ABSORBED_NULLS:
        break;
    default:
        return -EINVAL;
    }
    for (i = 0; i < n_rounds; ++i) {
        if ((result = passacre_gen_absorb(state, nulls, sizeof nulls))) {
            return result;
        }
    }
    state->mode = PASSACRE_GEN_ABSORBED_NULLS;
    return 0;
}


int
passacre_gen_squeeze(struct passacre_gen_state *state, unsigned char *output, size_t n_bytes)
{
    int just_started = 0;
    switch (state->mode) {
    case PASSACRE_GEN_ABSORBED_PASSWORD:
    case PASSACRE_GEN_ABSORBED_NULLS:
        just_started = 1;
        state->mode = PASSACRE_GEN_SQUEEZING;
        break;
    case PASSACRE_GEN_SQUEEZING:
        break;
    default:
        return -EINVAL;
    }
    switch (state->algorithm) {
    case PASSACRE_KECCAK:
        if (Squeeze(&state->hasher.keccak, output, n_bytes * 8)) {
            return -EINVAL;
        }
        break;

    case PASSACRE_SKEIN: {
        uint8_t input[64] = {0}, state_output[64];
        uint8_t tweak[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x3f};
        size_t i, half = n_bytes / 2, last_index = n_bytes - 1;
        unsigned char tmp, *output_start = output;
        struct _skein_prng_state *prng = &state->hasher.skein_prng;
        if (just_started) {
            uint8_t hash[64];
            if (skeinFinal(&state->hasher.skein, hash) != SKEIN_SUCCESS) {
                return -EINVAL;
            }
            threefishSetKey(&prng->threefish, Threefish512, (uint64_t *)hash, (uint64_t *)tweak);
            prng->bytes_remaining = 0;
        }
        while (n_bytes) {
            size_t to_copy = n_bytes > 64? 64 : n_bytes;
            if (!prng->bytes_remaining) {
                input[0] = 0;
                threefishEncryptBlockBytes(&prng->threefish, input, state_output);
                input[0] = 1;
                threefishEncryptBlockBytes(&prng->threefish, input, prng->buffer);
                threefishSetKey(&prng->threefish, Threefish512, (uint64_t *)state_output, (uint64_t *)tweak);
                prng->bytes_remaining = 64;
            }
            if (to_copy > prng->bytes_remaining) {
                to_copy = prng->bytes_remaining;
            }
            memcpy(output, prng->buffer + (64 - prng->bytes_remaining), to_copy);
            prng->bytes_remaining -= to_copy;
            n_bytes -= to_copy;
            output += to_copy;
        }
        /* reverse the bytes returned */
        for (i = 0; i < half; ++i) {
            tmp = output_start[i];
            output_start[i] = output_start[last_index - i];
            output_start[last_index - i] = tmp;
        }
        break;
    }

    default:
        return -EINVAL;
    }

    return 0;
}
