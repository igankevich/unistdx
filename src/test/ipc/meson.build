foreach conf : [
	['identity', [unistdx,gtest]],
	['process', [unistdx,gtest]],
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
