foreach conf : [
    ['system', [unistdx,gtest]],
    ['users', [unistdx,gtest], [[], [sysconf_stub]]],
]
    test_name = join_paths('util', conf[0])
    executable_name = conf[0] + '_test'
    source_name = executable_name + '.cc'
    stubs_list = conf.length() <= 2 ? [[]] : conf[2]
    foreach libs : stubs_list
        suffix = ''
        foreach l : libs
            tmp = l.full_path().split('lib')
            suffix += '--' + tmp[tmp.length()-1].split('.so')[0]
        endforeach
        test(
            test_name + suffix,
            executable(
                executable_name + suffix.underscorify(),
                sources: source_name,
                include_directories: srcdir,
                dependencies: conf[1],
                link_with: libs
            )
        )
    endforeach
endforeach

# backtrace test + stubs {{{
if config.get('UNISTDX_HAVE_BACKTRACE', false)
    foreach libs : [[], [backtrace_symbols_stub], [backtrace_symbols_stub_2]]
        suffix = ''
        foreach l : libs
            tmp = l.full_path().split('lib')
            suffix += '--' + tmp[tmp.length()-1].split('.so')[0]
        endforeach
        backtrace_test = executable(
            'backtrace-test' + suffix.underscorify(),
            sources: 'backtrace_test.cc',
            include_directories: srcdir,
            dependencies: [unistdx,threads],
            link_with: libs
        )
        foreach type : ['signal', 'terminate']
            test(
                'util/backtrace' + suffix + ' ' + type,
                backtrace_test,
                args: [type]
            )
        endforeach
    endforeach
endif
# }}}
