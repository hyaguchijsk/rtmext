_comp_rtm_pkg_list(){
    local pkgnames
    COMPREPLY=()
    pkgnames="`rtmpack list-names`"
    IFS=$'\n'
    COMPREPLY=($(compgen -W "${pkgnames}" -- $1))
    unset IFS
    return 0
}

_comp_rtm_file_list(){
    local pkgpath filenames
    COMPREPLY=()
    pkgpath="`rtmpack find $1`"
    filenames=$(for fname in `ls -1 ${pkgpath}/$2 2> /dev/null`; do echo ${fname/\/*\//}; done)
    COMPREPLY=($(compgen -W "${filenames}" -- $3))
    return 0
}

_comp_rtm_pkg_and_xml_list(){
    local pkgnames filenames
    COMPREPLY=()
    pkgnames="`rtmpack list-names`"
    filenames=$(for fname in `ls -1 ./*.xml 2> /dev/null`; do echo ${fname/.\//}; done)
    IFS=$'\n'
    COMPREPLY=($(compgen -W "${pkgnames}
${filenames}" -- $1))
    unset IFS
    return 0
}

comp_rtmpack(){
    arg=${COMP_WORDS[COMP_CWORD]}
    COMPREPLY=()

    case ${COMP_CWORD} in
	1)
	    COMPREPLY=($(compgen -W "find depend list-names" -- $arg))
	    return 0
	    ;;
	2)
	    local prev
	    prev=${COMP_WORDS[COMP_CWORD-1]}
	    if [[ "${prev}" != "list-names" ]]; then
		_comp_rtm_pkg_list ${arg}
	    fi
	    return 0
	    ;;
	*)
	    ;;
    esac
    return 0
}

comp_rtmcd()
{
    local arg
    arg=${COMP_WORDS[COMP_CWORD]}
    _comp_rtm_pkg_list ${arg}
    return 0
}

comp_rtmrun()
{
    arg=${COMP_WORDS[COMP_CWORD]}
    COMPREPLY=()

    case ${COMP_CWORD} in
	1)
	    _comp_rtm_pkg_list ${arg}
	    return 0
	    ;;
	2)
	    local prev
	    prev=${COMP_WORDS[COMP_CWORD-1]}
	    _comp_rtm_file_list ${prev} "" ${arg}
	    return 0
	    ;;
	*)
	    ;;
    esac
    return 0
}

comp_rtmlaunch()
{
    arg=${COMP_WORDS[COMP_CWORD]}
    COMPREPLY=()

    case ${COMP_CWORD} in
	1)
	    _comp_rtm_pkg_and_xml_list $arg
	    return 0
	    ;;
	2)
	    local prev
	    prev=${COMP_WORDS[COMP_CWORD-1]}
	    _comp_rtm_file_list ${prev} "*.xml" ${arg}
	    return 0
	    ;;
	*)
	    ;;
    esac
    return 0
}

complete -F comp_rtmpack rtmpack
complete -F comp_rtmcd rtmcd
complete -F comp_rtmcd rtmmake
complete -F comp_rtmrun rtmrun
complete -F comp_rtmlaunch rtmlaunch

