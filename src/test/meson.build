subdir('stub')
subdir('base')
subdir('fs')
subdir('io')
subdir('ipc')
subdir('it')
subdir('net')
subdir('util')

executable(
    'posix-test',
    sources: 'posix_test.cc',
    include_directories: srcdir,
)

# test coverage {{{
coverage = configuration_data()
lcov = find_program('lcov', required: false)
if lcov.found()
    coverage.set('lcov_path', lcov.path())
endif
genhtml = find_program('genhtml', required: false)
if genhtml.found()
    coverage.set('genhtml_path', genhtml.path())
endif
lcovrc_options = [
    '--rc lcov_branch_coverage=1',
    '--rc genhtml_branch_coverage=1',
]
lcov_options = [
    '--gcov-tool',
    join_paths(meson.current_build_dir(), 'gcov-tool')
]
genhtml_options = [
    '--demangle-cpp'
]
coverage.set('lcovrc_options', ' '.join(lcovrc_options))
coverage.set('lcov_options', ' '.join(lcov_options))
coverage.set('genhtml_options', ' '.join(genhtml_options))
configure_file(
    input: 'coverage.in',
    output: 'coverage',
    configuration: coverage
)
gcov = find_program('gcov', required: false)
gcov_tool = configuration_data()
gcov_tool_cmd = gcov.path()
if cpp.get_id() == 'clang'
    llvm_cov = find_program('llvm-cov', required: false)
    if llvm_cov.found()
        gcov_tool_cmd = llvm_cov.path() + ' gcov'
    endif
endif
gcov_tool.set('gcov_tool_cmd', gcov_tool_cmd)
configure_file(
    input: 'gcov-tool.in',
    output: 'gcov-tool',
    configuration: gcov_tool
)
if lcov.found() and genhtml.found()
    run_target(
        'test-lcov',
        command: join_paths(meson.current_build_dir(), 'coverage')
    )
endif
# }}}
# gcovr {{{
gcovr = find_program('gcovr', required: false)
if gcovr.found()
    run_target(
        'test-gcovr',
        command: [
            gcovr,
            '--branches',
            '--exclude-unreachable-branches',
            '-j',
            '--html-details',
            '--root',
            join_paths(meson.source_root(), 'src'),
            '--object-directory',
            meson.build_root(),
            '--exclude=.*test.*',
            '--exclude=/usr/include/.*',
            '--gcov-exclude=.*test.*',
            '--gcov-exclude=/usr/include/.*',
            '--print-summary',
            '--output',
            join_paths(meson.build_root(), 'gcovr', 'index.html')
        ]
    )
endif
# }}}
