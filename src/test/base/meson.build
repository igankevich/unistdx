foreach conf : [
	['cmdline', [unistdx,gtest]],
	['sha1', [unistdx,gtest]],
	['spin_mutex', [threads,unistdx,gtest]],
	['uint128', [unistdx,gtest]],
	['websocket', [unistdx,gtest]],
]
	test_name = join_paths('base', conf[0])
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

benchmark(
	'base/sha1',
	executable(
		'sha1-benchmark',
		sources: 'sha1_benchmark.cc',
		include_directories: srcdir,
		dependencies: [unistdx]
	)
)

