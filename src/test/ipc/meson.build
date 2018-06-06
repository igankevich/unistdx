foreach conf : [
	['execute', [unistdx,gtest]],
	['identity', [unistdx,gtest]],
	['proc_status', [unistdx,gtest]],
	['process', [unistdx,gtest]],
	['process_group', [unistdx,gtest]],
	['semaphore', [threads,unistdx,gtest]],
	['sharedmem', [threads,unistdx,gtest]],
	['signal', [unistdx,gtest]],
]
	test_name = join_paths('ipc', conf[0])
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

test(
	'ipc/identity-stub',
	executable(
		'identity_stub_test',
		sources: 'identity_stub_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx,gtest]
	),
	env: ['LD_PRELOAD=' + setgid_stub.full_path()]
)
