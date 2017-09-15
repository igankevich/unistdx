test(
	'ipc/semaphore',
	executable(
		'semaphore-test',
		sources: 'semaphore_test.cc',
		include_directories: srcdir,
		dependencies: [threads,unistdx,gtest],
	)
)

test(
	'net/socket',
	executable(
		'socket-test',
		sources: 'socket_test.cc',
		include_directories: srcdir,
		dependencies: [threads,unistdx,gtest],
	)
)

test(
	'base/spin_mutex',
	executable(
		'spin-mutex-test',
		sources: 'spin_mutex_test.cc',
		include_directories: srcdir,
		dependencies: [threads,unistdx,gtest],
	)
)

test(
	'ipc/sharedmem',
	executable(
		'sharedmem-test',
		sources: 'sharedmem_test.cc',
		include_directories: srcdir,
		dependencies: [threads,unistdx,gtest],
	)
)

test(
	'net/endpoint',
	executable(
		'endpoint-test',
		sources: 'endpoint_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx],
	)
)

test(
	'io/poller',
	executable(
		'poller-test',
		sources: 'poller_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx],
	)
)

test(
	'filesystem',
	executable(
		'filesystem-test',
		sources: 'filesystem_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'ipc/process',
	executable(
		'process-test',
		sources: 'process_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx],
	)
)

test(
	'ipc/identity',
	executable(
		'identity-test',
		sources: 'identity_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx],
	)
)

test(
	'io/fildesbuf',
	executable(
		'buffer-test',
		sources: 'buffer_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx],
		cpp_args: ['-DUNISTDX_DEBUG_PACKETBUF']
	)
)

test(
	'util/users',
	executable(
		'users-test',
		sources: 'users_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx,gtest],
	)
)

executable(
	'POSIX',
	sources: 'posix_test.cc',
	include_directories: srcdir,
)

test(
	'util/system',
	executable(
		'system-test',
		sources: 'system_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'fs/file_mutex',
	executable(
		'file-mutex-test',
		sources: 'file_mutex_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'ipc/signal',
	executable(
		'signal-test',
		sources: 'signal_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'fs/path',
	executable(
		'path-test',
		sources: 'path_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'fs/canonical_path',
	executable(
		'canonical-path-test',
		sources: 'canonical_path_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'net/ifaddr',
	executable(
		'ifaddr-test',
		sources: 'ifaddr_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'net/ipv4_addr',
	executable(
		'ipv4-addr-test',
		sources: 'ipv4_addr_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'net/endian',
	executable(
		'endian-test',
		sources: 'endian_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'net/base64',
	executable(
		'base64-test',
		sources: ['base64_test.cc'],
		include_directories: srcdir,
		dependencies: [gtest, unistdx]
	)
)

test(
	'base/uint128 (builtin)',
	executable(
		'uint128-test',
		sources: 'uint128_test.cc',
		include_directories: srcdir,
		dependencies: gtest
	)
)

test(
	'base/uint128 (emulate)',
	executable(
		'uint128-emulate-test',
		sources: 'uint128_test.cc',
		cpp_args: '-DUNISTDX_TEST_FORCE_CUSTOM_UINT128',
		include_directories: srcdir,
		dependencies: gtest
	)
)

test(
	'bits/paired_iterator',
	executable(
		'paired-iterator-test',
		sources: 'paired_iterator_test.cc',
		include_directories: srcdir,
		dependencies: gtest
	)
)

test(
	'it/queue_pusher',
	executable(
		'queue-pusher-test',
		sources: 'queue_pusher_test.cc',
		include_directories: srcdir,
		dependencies: gtest
	)
)

test(
	'base/adapt_engine',
	executable(
		'adapt-engine-test',
		sources: 'adapt_engine_test.cc',
		include_directories: srcdir,
		dependencies: gtest
	)
)

test(
	'base/n_random_bytes',
	executable(
		'n-random-bytes-test',
		sources: 'n_random_bytes_test.cc',
		include_directories: srcdir,
		dependencies: gtest
	)
)

test(
	'base/cmdline',
	executable(
		'cmdline-test',
		sources: 'cmdline_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx]
	)
)

test(
	'base/sha1',
	executable(
		'sha1-test',
		sources: 'sha1_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx]
	)
)

benchmark(
	'base/sha1',
	executable(
		'sha1-benchmark',
		sources: 'sha1_benchmark.cc',
		include_directories: srcdir,
		dependencies: [unistdx]
	)
)

test(
	'base/websocket',
	executable(
		'websocket-test',
		sources: 'websocket_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx]
	)
)

if config.get('UNISTDX_HAVE_BACKTRACE', false)
	backtrace_test = executable(
		'backtrace-test',
		sources: 'backtrace_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx,threads],
	)
	foreach type : ['signal', 'terminate']
		test(
			'util/backtrace ' + type,
			backtrace_test,
			args: [type]
		)
	endforeach
endif

test(
	'io/fildes',
	executable(
		'fildes-test',
		sources: 'fildes_test.cc',
		include_directories: srcdir,
		dependencies: [gtest,unistdx]
	)
)
