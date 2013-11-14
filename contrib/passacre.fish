

function __fish_passacre_using_command
	set cmd (commandline -opc)
	set argvpos 1
	set argvcount (count $argv)
	for c in $cmd
		if [ $argvpos -le $argvcount ]
			set arg $argv[$argvpos]
		else
			set arg --
		end
		switch $c
		case '-*'
			continue
		case $arg
			set argvpos (math $argvpos + 1)
		case '*'
			return 1
		end
	end
	test $argvpos -gt $argvcount
	return $status
end

function __fish_passacre_sites
	passacre site | cut -d: -f1
end

function __fish_passacre_schemata
	passacre schema | cut -d: -f1
end

    
complete -f -c passacre -n '__fish_passacre_using_command passacre config' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre config' -s s -l site -a '(__fish_passacre_sites)'
complete -f -c passacre -n '__fish_passacre_using_command passacre config' -s s -l site -d 'the site to operate on or omitted for global config'
complete -f -c passacre -n '__fish_passacre_using_command passacre config' -s a -l hashed -d 'hash the site name'
complete -f -c passacre -n '__fish_passacre_using_command passacre config' -s c -l confirm -d 'confirm prompted password'
complete -f -c passacre -n '__fish_passacre_using_command passacre entropy' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre entropy'  -l schema -d 'show entropy by schema instead of by site'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s o -l override-config -a '()'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s o -l override-config -d 'a JSON dictionary of config values to override'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s u -l username -a '()'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s u -l username -d 'username for the site'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s n -l no-newline -d "don't write a newline after the password"
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s c -l confirm -d 'confirm prompted password'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s C -l copy -d 'put the generated password on the clipboard'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s w -l timeout -a '()'
complete -f -c passacre -n '__fish_passacre_using_command passacre generate' -s w -l timeout -d 'clear the clipboard after N seconds'
complete -f -c passacre -n '__fish_passacre_using_command passacre info' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre init' -s h -l help -d 'show this help message and exit'
complete  -c passacre -n '__fish_passacre_using_command passacre init' -s y -l from-yaml 
complete -f -c passacre -n '__fish_passacre_using_command passacre init' -s y -l from-yaml -d 'optional input YAML config file to convert from'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema add' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema remove' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema set-name' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema set-value' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema' -a 'add' -d 'add a schema'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema' -a 'remove' -d 'remove a schema'
complete -f -c passacre -n '__fish_passacre_using_command passacre schema' -a 'set-name' -d "change a schema's name"
complete -f -c passacre -n '__fish_passacre_using_command passacre schema' -a 'set-value' -d "change a schema's value"
complete -f -c passacre -n '__fish_passacre_using_command passacre site add' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site add' -s N -l new-schema -a '()'
complete -f -c passacre -n '__fish_passacre_using_command passacre site add' -s N -l new-schema -d 'a schema value for the new schema'
complete -f -c passacre -n '__fish_passacre_using_command passacre site config' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site config' -s a -l hashed -d 'hash the site name'
complete -f -c passacre -n '__fish_passacre_using_command passacre site config' -s c -l confirm -d 'confirm prompted password'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash' -s m -l method -a 'keccak skein'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash' -s m -l method -d 'which hash method to use'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash' -s n -l no-newline -d "don't write a newline after the hash"
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash' -s c -l confirm -d 'confirm prompted password'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash-all' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash-all' -s m -l method -a 'keccak skein'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash-all' -s m -l method -d 'which hash method to use'
complete -f -c passacre -n '__fish_passacre_using_command passacre site hash-all' -s c -l confirm -d 'confirm prompted password'
complete -f -c passacre -n '__fish_passacre_using_command passacre site remove' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site set-name' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site set-schema' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre site'  -l by-schema -d 'list sites organized by schema'
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -s a -l hashed -d 'hash the site name'
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -s c -l confirm -d 'confirm prompted password'
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -a 'add' -d 'add a site to a config file'
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -a 'config' -d "change a site's configuration"
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -a 'hash' -d "hash a site's name"
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -a 'hash-all' -d 'hash all non-hashed sites'
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -a 'remove' -d 'remove a site from a config file'
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -a 'set-name' -d "change a site's domain"
complete -f -c passacre -n '__fish_passacre_using_command passacre site' -a 'set-schema' -d "change a site's schema"
complete -f -c passacre -n '__fish_passacre_using_command passacre' -s h -l help -d 'show this help message and exit'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -s V -l version -d "show program's version number and exit"
complete -f -c passacre -n '__fish_passacre_using_command passacre' -s v -l verbose -d 'increase output on errors'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -s f -l config -a '()'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -s f -l config -d 'specify a config file to use'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -a 'config' -d 'view/change global configuration'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -a 'entropy' -d "display each site's password entropy"
complete -f -c passacre -n '__fish_passacre_using_command passacre' -a 'generate' -d 'generate a password'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -a 'info' -d 'information about the passacre environment'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -a 'init' -d 'initialize an sqlite config'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -a 'schema' -d 'actions on schemata'
complete -f -c passacre -n '__fish_passacre_using_command passacre' -a 'site' -d 'actions on sites'
