libunistdx_src += [
    files('error.cc')
]

libunistdx_tests += [
    [files('log_test.cc')],
]

if with_debug
    libunistdx_tests += [
        [files('clock_test.cc')],
        [files('error_test.cc')],
    ]
endif

install_headers(
    'clock',
    'error',
    'log',
    'time',
    subdir: join_paths(meson.project_name(), 'system')
)