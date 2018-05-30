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
configure_file(
	input: 'coverage.in',
	output: 'coverage',
	configuration: coverage
)
if lcov.found() and genhtml.found()
	run_target(
		'test-coverage',
		command: join_paths(meson.current_build_dir(), 'coverage')
	)
endif
# }}}
