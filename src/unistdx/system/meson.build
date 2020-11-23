libunistdx_src += files([
    'error.cc',
    'nss.cc',
    'resource.cc',
])

libunistdx_tests += [
    [files('log_test.cc')],
    [files('resource_test.cc')],
]

if with_debug
    libunistdx_tests += [
        [files('clock_test.cc')],
        [files('error_test.cc')],
        #[files('users_test.cc'), [[], [sysconf_stub]]],
    ]
endif

if config.get('UNISTDX_HAVE_BACKTRACE', false)
    libunistdx_tests += [
        [files('backtrace_test.cc'),
         [[], [backtrace_symbols_stub], [backtrace_symbols_stub_2]],
         [['signal'], ['terminate']]],
    ]
endif

install_headers(
    'clock',
    'error',
    'log',
    'nss',
    'time',
    'resource',
    subdir: join_paths(meson.project_name(), 'system')
)
