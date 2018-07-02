foreach conf : [
	['execute', [unistdx,gtest]],
	['identity', [unistdx,gtest], [[], [setgid_stub]]],
	['process', [unistdx,gtest]],
	['process_group', [unistdx,gtest]],
	['process_status', [unistdx,gtest]],
	['semaphore', [threads,unistdx,gtest]],
	['shared_memory_segment', [threads,unistdx,gtest]],
	['signal', [unistdx,gtest]],
]
	test_name = join_paths('ipc', conf[0])
	executable_name = conf[0] + '_test'
	source_name = executable_name + '.cc'
	stubs_list = conf.length() <= 2 ? [[]] : conf[2]
	foreach libs : stubs_list
		suffix = ''
		foreach l : libs
			tmp = l.full_path().split('lib')
			suffix += '--' + tmp[tmp.length()-1].split('.so')[0]
		endforeach
		test(
			test_name + suffix,
			executable(
				executable_name + suffix,
				sources: source_name,
				include_directories: srcdir,
				dependencies: conf[1],
				link_with: libs
			)
		)
	endforeach
endforeach
