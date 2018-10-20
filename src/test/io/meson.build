foreach conf : [
	['epoll_event', [unistdx,gtest], []],
	['fildes', [unistdx,gtest], []],
	['fildesbuf', [unistdx,gtest], [
		'-DUNISTDX_DEBUG_PACKETBUF',
		'-DUNISTDX_DEBUG_FILDESBUF'
	]],
	['poll_event', [unistdx,gtest], []],
	['poller', [unistdx,gtest], []],
	['two_way_pipe', [unistdx,gtest], []],
	['memory_mapping', [unistdx,gtest], []],
]
	test_name = join_paths('io', conf[0])
	executable_name = conf[0] + '_test'
	source_name = executable_name + '.cc'
	test(
		test_name,
		executable(
			executable_name,
			sources: source_name,
			include_directories: srcdir,
			dependencies: conf[1],
			cpp_args: conf[2]
		)
	)
endforeach

benchmark(
	'base/memory_mapping',
	executable(
		'memory-mapping-benchmark',
		sources: 'memory_mapping_benchmark.cc',
		include_directories: srcdir,
		dependencies: [unistdx]
	)
)

