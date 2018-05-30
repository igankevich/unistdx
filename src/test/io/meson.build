foreach conf : [
	['fildesbuf', [unistdx,gtest], [
		'-DUNISTDX_DEBUG_PACKETBUF',
		'-DUNISTDX_DEBUG_FILDESBUF'
	]],
	['poller', [unistdx,gtest], []],
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
