subdir('base')
subdir('fs')
subdir('io')
subdir('ipc')
subdir('it')
subdir('net')
subdir('util')

executable(
	'POSIX',
	sources: 'posix_test.cc',
	include_directories: srcdir,
)

benchmark(
	'base/sha1',
	executable(
		'sha1-benchmark',
		sources: 'base/sha1_benchmark.cc',
		include_directories: srcdir,
		dependencies: [unistdx]
	)
)

if config.get('UNISTDX_HAVE_BACKTRACE', false)
	backtrace_test = executable(
		'backtrace-test',
		sources: 'util/backtrace_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx,threads],
		cpp_args: ['-fno-omit-frame-pointer']
	)
	foreach type : ['signal', 'terminate']
		test(
			'util/backtrace-' + type,
			backtrace_test,
			args: [type]
		)
	endforeach
endif

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
