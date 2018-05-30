all_tests = [
	['base/cmdline', [unistdx,gtest], []],
	['base/sha1', [unistdx,gtest], []],
	['base/spin_mutex', [threads,unistdx,gtest], []],
	['base/uint128', [unistdx,gtest], []],
	['base/websocket', [unistdx,gtest], []],
	['fs/file_mutex', [unistdx,gtest], []],
	['fs/idirectory', [unistdx,gtest], []],
	['fs/idirtree', [unistdx,gtest], []],
	['fs/mkdirs', [unistdx,gtest], []],
	['fs/path', [unistdx,gtest], []],
	['io/fildesbuf', [unistdx,gtest], [
		'-DUNISTDX_DEBUG_PACKETBUF',
		'-DUNISTDX_DEBUG_FILDESBUF'
	]],
	['io/poller', [unistdx,gtest], []],
	['ipc/identity', [unistdx,gtest], []],
	['ipc/process', [unistdx,gtest], []],
	['ipc/semaphore', [threads,unistdx,gtest], []],
	['ipc/sharedmem', [threads,unistdx,gtest], []],
	['ipc/signal', [unistdx,gtest], []],
	['it/field_iterator', [unistdx,gtest], []],
	['it/queue_pusher', [unistdx,gtest], []],
	['net/base64', [unistdx,gtest], []],
	['net/byte_swap', [unistdx,gtest], []],
	['net/endian', [unistdx,gtest], []],
	['net/endpoint', [unistdx,gtest], []],
	['net/ifaddr', [unistdx,gtest], []],
	['net/ipv4_addr', [unistdx,gtest], []],
	['net/netlink_poller', [threads,unistdx,gtest], []],
	['net/socket', [threads,unistdx,gtest], []],
	['util/system', [unistdx,gtest], []],
	['util/users', [unistdx,gtest], []],
]

foreach conf : all_tests
	test_name = conf[0]
	executable_name = test_name + '_test'
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

executable(
	'POSIX',
	sources: 'posix_test.cc',
	include_directories: srcdir,
)

benchmark(
	'base/sha1',
	executable(
		'sha1-benchmark',
		sources: 'base/sha1_benchmark.cc',
		include_directories: srcdir,
		dependencies: [unistdx]
	)
)

if config.get('UNISTDX_HAVE_BACKTRACE', false)
	backtrace_test = executable(
		'backtrace-test',
		sources: 'util/backtrace_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx,threads],
		cpp_args: ['-fno-omit-frame-pointer']
	)
	foreach type : ['signal', 'terminate']
		test(
			'util/backtrace-' + type,
			backtrace_test,
			args: [type]
		)
	endforeach
endif

# test coverage {{{
coverage = configuration_data()
lcov = find_program('lcov', required: false)
if lcov.found()
	coverage.set('lcov_path', lcov.path())
endif
genhtml = find_program('genhtml', required: false)
if genhtml.found()
	coverage.set('genhtml_path', genhtml.path())
endif
configure_file(
	input: 'coverage.in',
	output: 'coverage',
	configuration: coverage
)
if lcov.found() and genhtml.found()
	run_target(
		'test-coverage',
		command: join_paths(meson.current_build_dir(), 'coverage')
	)
endif
# }}}
