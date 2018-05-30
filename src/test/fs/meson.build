foreach conf : [
	['file_mutex', [unistdx,gtest]],
	['idirectory', [unistdx,gtest]],
	['idirtree', [unistdx,gtest]],
	['mkdirs', [unistdx,gtest]],
	['path', [unistdx,gtest]],
]
	test_name = join_paths('fs', conf[0])
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
