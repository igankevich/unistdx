project(
	'unistdx',
	'cpp',
	version: '3.2',
	meson_version: '>=0.31',
	default_options: ['cpp_std=c++11']
)

if not get_option('buildtype').contains('debug')
	add_global_arguments('-DNDEBUG', language: 'cpp')
endif

# compile-time configuration {{{
config = configuration_data()
cpp = meson.get_compiler('cpp')
builtin = 'int main() { @0@(0); }'
foreach suffix : ['', 'l', 'll']
	if cpp.compiles(builtin.format('__builtin_popcount' + suffix))
		config.set('UNISTDX_HAVE_BUILTIN_POPCOUNT' + suffix.to_upper(), true)
	endif
endforeach
foreach suffix : ['16', '32', '64']
	if cpp.compiles(builtin.format('__builtin_bswap' + suffix))
		config.set('UNISTDX_HAVE_BUILTIN_SWAP' + suffix, true)
	endif
endforeach
if host_machine.endian() == 'little'
	config.set('UNISTDX_LITTLE_ENDIAN', true)
elif host_machine.endian() == 'big'
	config.set('UNISTDX_BIG_ENDIAN', true)
else
	message('unknown endiannes')
endif
if cpp.has_type('long double')
	config.set('UNISTDX_HAVE_LONG_DOUBLE', true)
endif
if cpp.has_type('long long')
	config.set('UNISTDX_HAVE_LONG_LONG', true)
endif
if cpp.has_type('__int128')
	config.set('UNISTDX_HAVE_INT128', true)
endif
if cpp.has_header('ifaddrs.h')
	config.set('UNISTDX_HAVE_IFADDRS', true)
endif
if cpp.has_header_symbol('sys/ioctl.h', 'FIONREAD')
	config.set('UNISTDX_HAVE_FIONREAD', true)
endif
if cpp.has_header_symbol('dirent.h', 'DTTOIF')
	config.set('UNISTDX_HAVE_DTTOIF', true)
endif
if cpp.has_member('passwd', 'pw_gecos', prefix: '#include <pwd.h>')
	config.set('UNISTDX_HAVE_GECOS', true)
endif
if cpp.has_header_symbol('poll.h', 'POLLRDHUP')
	config.set('UNISTDX_HAVE_POLLRDHUP', true)
endif
foreach name : [
		'SOCK_NONBLOCK',
		'SOCK_CLOEXEC',
		'SCM_CREDENTIALS',
		'SCM_RIGHTS',
		'SO_PEERCRED',
		'accept4'
	]
	if cpp.has_header_symbol('sys/socket.h', name)
		config.set('UNISTDX_HAVE_' + name.to_upper(), true)
	endif
endforeach
if cpp.has_header_symbol('netinet/tcp.h', 'TCP_USER_TIMEOUT')
	config.set('UNISTDX_HAVE_TCP_USER_TIMEOUT', true)
endif
if cpp.has_header_symbol('fcntl.h', 'F_SETNOSIGPIPE')
	config.set('UNISTDX_HAVE_SETNOSIGPIPE', true)
endif
if cpp.has_header('linux/netlink.h')
	config.set('UNISTDX_HAVE_NETLINK', true)
endif
foreach name : [
		'O_CLOEXEC',
		'O_DIRECT',
		'O_DIRECTORY',
		'O_LARGEFILE',
		'O_NOATIME',
		'O_NOFOLLOW',
		'O_PATH',
		'O_RSYNC',
		'O_TMPFILE',
	]
	if cpp.has_header_symbol('fcntl.h', name)
		config.set('UNISTDX_HAVE_' + name, true)
	endif
endforeach
foreach name : ['SHM_HUGETLB', 'SHM_HUGE_2MB', 'SHM_HUGE_1GB', 'SHM_NORESERVE']
	if cpp.has_header_symbol('sys/shm.h', name)
		config.set('UNISTDX_HAVE_' + name, true)
	endif
endforeach
foreach name : ['SIGPOLL', 'SIGSTKFLT', 'SIGPWR', 'SIGWINCH']
	if cpp.has_header_symbol('signal.h', name)
		config.set('UNISTDX_HAVE_' + name, true)
	endif
endforeach
if cpp.has_header('semaphore.h')
	config.set('UNISTDX_HAVE_POSIX_SEMAPHORES', true)
endif
if cpp.has_header('sys/sem.h')
	config.set('UNISTDX_HAVE_SYSV_SEMAPHORES', true)
endif
if cpp.has_header_symbol('sys/sem.h', 'semtimedop')
	config.set('UNISTDX_HAVE_SEMTIMEDOP', true)
endif
if cpp.has_header_symbol('semaphore.h', 'sem_timedwait')
	config.set('UNISTDX_HAVE_SEM_TIMEDWAIT', true)
endif
if cpp.has_header('execinfo.h')
	config.set('UNISTDX_HAVE_BACKTRACE', true)
	if cpp.get_id() == 'gcc' or cpp.has_argument('-rdynamic')
		add_global_link_arguments('-rdynamic', language: 'cpp')
	endif
	if cpp.has_header('cxxabi.h')
		config.set('UNISTDX_HAVE_CXXABI', true)
	endif
endif
foreach name : ['getpwnam_r' , 'getpwuid_r']
	if cpp.has_header_symbol('pwd.h', name)
		config.set('UNISTDX_HAVE_' + name, true)
	endif
endforeach
foreach name : ['getgrnam_r', 'getgrgid_r']
	if cpp.has_header_symbol('grp.h', name)
		config.set('UNISTDX_HAVE_' + name, true)
	endif
endforeach
config.set('UNISTDX_VERSION', meson.project_version())
if cpp.has_header_symbol('sys/mman.h', 'mmap')
	config.set('UNISTDX_HAVE_MMAP', true)
endif
if cpp.has_header_symbol('sys/epoll.h', 'epoll_create')
	config.set('UNISTDX_HAVE_EPOLL', true)
endif
if cpp.has_header_symbol('sys/sendfile.h', 'sendfile')
	config.set('UNISTDX_HAVE_SENDFILE', true)
endif
if cpp.has_header_symbol('sys/syscall.h', '__NR_copy_file_range')
	config.set('UNISTDX_HAVE_COPY_FILE_RANGE', true)
endif
if cpp.has_header_symbol('sched.h', 'clone')
	config.set('UNISTDX_HAVE_CLONE', true)
endif
if cpp.has_header_symbol('sys/prctl.h', 'prctl')
	config.set('UNISTDX_HAVE_PRCTL', true)
endif
if cpp.has_header_symbol('unistd.h', 'pipe2')
	config.set('UNISTDX_HAVE_PIPE2', true)
endif
# }}}

threads = dependency('threads')
gtest = dependency('gtest', main: true)

subdir('src')
subdir('doc')

# pkgconfig {{{
pkgconfig = import('pkgconfig')
pkgconfig.generate(
	libraries: libunistdx_shared,
	version: meson.project_version(),
	name: meson.project_name(),
	description: 'C++ wrappers for libc',
	filebase: meson.project_name()
)
# }}}
# cppcheck {{{
cppcheck = find_program('cppcheck', required: false)
if cppcheck.found()
	run_target(
		'cppcheck',
		command: [
			cppcheck,
			'--enable=all',
			'--quiet',
			'--force',
			'--language=c++',
			'--std=c++11',
			'--template=gcc',
			'-I' + join_paths(meson.source_root(), 'src'),
			'-I' + join_paths(meson.build_root(), 'src'),
			join_paths(meson.source_root(), 'src'),
		]
	)
endif
# }}}
# clang-check {{{
clang_check = find_program('clang-check', required: false)
if clang_check.found()
	run_target(
		'clang-check',
		command: [clang_check, '-analyze'] + libunistdx_src + [
			'--',
			'-I' + join_paths(meson.source_root(), 'src'),
			'-I' + join_paths(meson.build_root(), 'src'),
			'-std=c++11'
		]
	)
endif
# }}}
# clang-tidy {{{
clang_tidy = find_program('clang-tidy', required: false)
if clang_tidy.found()
	run_target(
		'clang-tidy',
		command: [clang_check] + libunistdx_src + [
			'--',
			'-I' + join_paths(meson.source_root(), 'src'),
			'-I' + join_paths(meson.build_root(), 'src'),
			'-std=c++11'
		]
	)
endif
# }}}
# archive {{{
archive_name = meson.project_name() + '-' + meson.project_version()
run_target(
	'archive',
	command: [
		'git',
		'archive',
		'-o',
		join_paths(meson.build_root(), archive_name + '.tar.xz'),
		'--prefix',
		archive_name + '/',
		'HEAD'
	]
)
# }}}
