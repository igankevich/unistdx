test(
	'stdx::spin_mutex, sys::sysv_semaphore',
	executable(
		'locks-test',
		sources: 'locks_test.cc',
		include_directories: srcdir,
		dependencies: [threads,unistdx],
	)
)

test(
	'sys::shared_mem',
	executable(
		'shmem-test',
		sources: 'shmem_test.cc',
		include_directories: srcdir,
		dependencies: [threads,unistdx],
	)
)

test(
	'sys::endpoint',
	executable(
		'endpoint-test',
		sources: 'endpoint_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx],
	)
)

test(
	'sys::event_poller',
	executable(
		'poller-test',
		sources: 'poller_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx],
	)
)

test(
	'sys::file, sys::direntry, sys::path',
	executable(
		'sys-test',
		sources: 'sys_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx],
	)
)

test(
	'sys::process',
	executable(
		'process-test',
		sources: 'process_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx],
	)
)

test(
	'sys::fildesbuf',
	executable(
		'buffer-test',
		sources: 'buffer_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx],
	)
)

test(
	'sys::users',
	executable(
		'users-test',
		sources: 'users_test.cc',
		include_directories: srcdir,
		dependencies: [unistdx],
	)
)

executable(
	'posix-compliance',
	sources: 'posix_test.cc',
	include_directories: srcdir,
)

test(
	'system-test',
	executable(
		'system-test',
		sources: 'system_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'file-mutex-test',
	executable(
		'file-mutex-test',
		sources: 'file_mutex_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)

test(
	'signal-test',
	executable(
		'signal-test',
		sources: 'signal_test.cc',
		include_directories: srcdir,
		dependencies: [gtest, unistdx],
	)
)
