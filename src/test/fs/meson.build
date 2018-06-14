foreach conf : [
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

# copy_file + stubs {{{
copy_file_test = executable(
	'copy_file_test',
	sources: 'copy_file_test.cc',
	include_directories: srcdir,
	dependencies: [unistdx,gtest]
)

stub1 = copy_file_range_stub.full_path()
stub2 = ':'.join([
	copy_file_range_stub.full_path(),
	sendfile_stub.full_path()
])

test('fs/copy_file', copy_file_test)
test('fs/copy_file-stub-1', copy_file_test, env: ['LD_PRELOAD=' + stub1])
test('fs/copy_file-stub-2', copy_file_test, env: ['LD_PRELOAD=' + stub2])
# }}}
