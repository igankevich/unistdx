libunistdx_src += files([
    'backtrace.cc',
    'group.cc',
    'system.cc',
    'user.cc',
])

install_headers(
    'backtrace',
    'group',
    'groupstream',
    'system',
    'user',
    'userstream',
    subdir: join_paths(meson.project_name(), 'util')
)

libunistdx_tests += [
    [files('system_test.cc')],
    [files('users_test.cc'), [[], [sysconf_stub]]],
]

if config.get('UNISTDX_HAVE_BACKTRACE', false)
    libunistdx_tests += [
        [files('backtrace_test.cc'),
         [[], [backtrace_symbols_stub], [backtrace_symbols_stub_2]],
         [['signal'], ['terminate']]],
    ]
endif
