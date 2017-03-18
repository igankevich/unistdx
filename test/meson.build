test(
	'stdx::spin_mutex, sys::sysv_semaphore',
	executable(
		'locks-test',
		sources: 'locks_test.cc',
		dependencies: libpthread,
		include_directories: srcdir,
	)
)

test(
	'sys::shared_mem',
	executable(
		'shmem-test',
		sources: 'shmem_test.cc',
		dependencies: libpthread,
		include_directories: srcdir,
	)
)

test(
	'sys::endpoint',
	executable(
		'endpoint-test',
		sources: 'endpoint_test.cc',
		include_directories: srcdir,
	)
)

test(
	'sys::event_poller',
	executable(
		'poller-test',
		sources: 'poller_test.cc',
		include_directories: srcdir,
	)
)

test(
	'sys::file, sys::direntry, sys::path',
	executable(
		'sys-test',
		sources: 'sys_test.cc',
		include_directories: srcdir,
	)
)

test(
	'sys::process',
	executable(
		'process-test',
		sources: 'process_test.cc',
		include_directories: srcdir,
	)
)

test(
	'sys::fildesbuf',
	executable(
		'buffer-test',
		sources: 'buffer_test.cc',
		include_directories: srcdir,
	)
)

test(
	'sys::users',
	executable(
		'users-test',
		sources: 'users_test.cc',
		include_directories: srcdir,
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
		dependencies: [gtest, unistdx],
		include_directories: srcdir,
	)
)
