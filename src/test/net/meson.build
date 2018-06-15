foreach conf : [
	['base64', [unistdx,gtest]],
	['bit_count', [unistdx,gtest]],
	['byte_swap', [unistdx,gtest]],
	['bytes', [unistdx,gtest]],
	['family', [unistdx,gtest]],
	['interface_address', [unistdx,gtest]],
	['ipv4_address', [unistdx,gtest]],
	['netlink_poller', [threads,unistdx,gtest]],
	['socket', [threads,unistdx,gtest]],
	['socket_address', [unistdx,gtest]],
]
	test_name = join_paths('net', conf[0])
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
