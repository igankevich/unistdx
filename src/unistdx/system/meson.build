libunistdx_src += files([
    'error.cc',
    'nss.cc',
    'resource.cc',
])

libdl = cpp.find_library('dl', required: false)
if libdl.found()
    libunistdx_src += files(['linker.cc'])
endif

libunistdx_tests += [
    [files('log_test.cc')],
    [files('resource_test.cc')],
]

libunistdx_tests += [
    [files('clock_test.cc')],
    [files('error_test.cc')],
    #[files('users_test.cc'), [[], [sysconf_stub]]],
]

if config.get('UNISTDX_HAVE_BACKTRACE', false)
    libunistdx_tests += [
        [files('backtrace_test.cc'),
         [[], [backtrace_symbols_stub], [backtrace_symbols_stub_2]],
         [['signal'], ['terminate']]],
    ]
endif

install_headers(
    'call',
    'clock',
    'error',
    'linker',
    'log',
    'nss',
    'resource',
    'time',
    subdir: join_paths(meson.project_name(), 'system')
)
