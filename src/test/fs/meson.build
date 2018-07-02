foreach conf : [
	['copy_file', [unistdx,gtest], [
		[],
		[copy_file_range_stub],
		[copy_file_range_stub, sendfile_stub],
	]],
	['file_mode', [unistdx,gtest]],
	['file_mutex', [unistdx,gtest]],
	['file_status', [unistdx,gtest]],
	['file_type', [unistdx,gtest]],
	['idirectory', [unistdx,gtest]],
	['idirtree', [unistdx,gtest]],
	['mkdirs', [unistdx,gtest]],
	['path', [unistdx,gtest]],
]
	test_name = join_paths('fs', conf[0])
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
				executable_name + suffix.underscorify(),
				sources: source_name,
				include_directories: srcdir,
				dependencies: conf[1],
				link_with: libs
			)
		)
	endforeach
endforeach
