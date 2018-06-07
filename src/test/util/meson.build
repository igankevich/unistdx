foreach conf : [
	['system', [unistdx,gtest]],
	['users', [unistdx,gtest]],
]
	test_name = join_paths('util', conf[0])
	executable_name = conf[0] + '_test'
	source_name = executable_name + '.cc'
	test(
		test_name,
		executable(
			executable_name,
			sources: source_name,
			include_directories: srcdir,
			dependencies: conf[1]
		)
	)
endforeach

if config.get('UNISTDX_HAVE_BACKTRACE', false)
	backtrace_test = executable(
		'backtrace-test',
		sources: 'backtrace_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx,threads],
		cpp_args: ['-fno-omit-frame-pointer']
	)
	foreach type : ['signal', 'terminate']
		test('util/backtrace-' + type, backtrace_test, args: [type])
		test(
			'util/backtrace-stub-' + type,
			backtrace_test,
			args: [type],
			env: [
				'LD_PRELOAD=' + backtrace_symbols_stub.full_path()
			]
		)
		test(
			'util/backtrace-stub-2-' + type,
			backtrace_test,
			args: [type],
			env: [
				'LD_PRELOAD=' + backtrace_symbols_stub_2.full_path()
			]
		)
	endforeach
endif

