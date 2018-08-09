project(
	'unistdx',
	'cpp',
	version: '0.4.8',
	meson_version: '>=0.46',
	default_options: ['cpp_std=c++11'],
	license: ['Public domain']
)

if not get_option('buildtype').contains('debug')
	add_global_arguments('-DNDEBUG', language: 'cpp')
endif

# compile-time configuration {{{
config = configuration_data()
cpp = meson.get_compiler('cpp')
foreach pair : [
	['bswap16', '0'],
	['bswap32', '0'],
	['bswap64', '0'],
	['popcount', '0'],
	['popcountl', '0'],
	['popcountll', '0'],
	['unreachable', ''],
]
	func = pair[0]
	args = pair[1]
	builtin = 'int main() { @0@(@1@); }'
	if cpp.compiles(builtin.format('__builtin_' + func, args))
		config.set('UNISTDX_HAVE_BUILTIN_' + func.to_upper(), true)
	endif
endforeach
if host_machine.endian() == 'little'
	config.set('UNISTDX_LITTLE_ENDIAN', true)
elif host_machine.endian() == 'big'
	config.set('UNISTDX_BIG_ENDIAN', true)
else
	error('unknown endiannes')
endif
foreach pair : [
	['__int128', 'int128'],
	['long double'],
	['long long'],
]
	type = pair[0]
	name = type.underscorify()
	if pair.length() > 1
		name = pair[1]
	endif
	if cpp.has_type(type)
		config.set('UNISTDX_HAVE_' + name.to_upper(), true)
	endif
endforeach
foreach pair : [
	['dirent.h', 'DTTOIF'],
	['execinfo.h', 'backtrace'],
	['fcntl.h', 'F_SETNOSIGPIPE'],
	['fcntl.h', 'O_CLOEXEC'],
	['fcntl.h', 'O_DIRECT'],
	['fcntl.h', 'O_DIRECTORY'],
	['fcntl.h', 'O_LARGEFILE'],
	['fcntl.h', 'O_NOATIME'],
	['fcntl.h', 'O_NOFOLLOW'],
	['fcntl.h', 'O_PATH'],
	['fcntl.h', 'O_RSYNC'],
	['fcntl.h', 'O_TMPFILE'],
	['grp.h', 'getgrgid_r'],
	['grp.h', 'getgrnam_r'],
	['netinet/tcp.h', 'TCP_USER_TIMEOUT'],
	['poll.h', 'POLLRDHUP'],
	['pwd.h', 'getpwnam_r'],
	['pwd.h', 'getpwuid_r'],
	['sched.h', 'clone'],
	['semaphore.h', 'sem_timedwait'],
	['signal.h', 'SIGPOLL'],
	['signal.h', 'SIGPWR'],
	['signal.h', 'SIGSTKFLT'],
	['signal.h', 'SIGWINCH'],
	['sys/ioctl.h', 'FIONREAD'],
	['sys/mman.h', 'mmap'],
	['sys/prctl.h', 'prctl'],
	['sys/sem.h', 'semtimedop'],
	['sys/sendfile.h', 'sendfile'],
	['sys/shm.h', 'SHM_HUGETLB'],
	['sys/shm.h', 'SHM_HUGE_1GB'],
	['sys/shm.h', 'SHM_HUGE_2MB'],
	['sys/shm.h', 'SHM_HUGE_SHIFT'],
	['sys/shm.h', 'SHM_NORESERVE'],
	['sys/socket.h', 'SCM_CREDENTIALS'],
	['sys/socket.h', 'SCM_RIGHTS'],
	['sys/socket.h', 'SOCK_CLOEXEC'],
	['sys/socket.h', 'SOCK_NONBLOCK'],
	['sys/socket.h', 'SO_PEERCRED'],
	['sys/socket.h', 'accept4'],
	['unistd.h', 'copy_file_range'],
	['unistd.h', 'pipe2'],
]
	header = pair[0]
	symbol = pair[1]
	if cpp.has_header_symbol(header, symbol)
		config.set('UNISTDX_HAVE_' + symbol.to_upper(), true)
	endif
endforeach
foreach pair : [
	['cxxabi.h', 'cxxabi'],
	['ifaddrs.h', 'ifaddrs'],
	['linux/netlink.h', 'netlink'],
	['semaphore.h', 'posix_semaphores'],
	['sys/epoll.h', 'epoll'],
	['sys/sem.h', 'sysv_semaphores'],
]
	header = pair[0]
	name = pair[1]
	if cpp.has_header(header)
		config.set('UNISTDX_HAVE_' + name.to_upper(), true)
	endif
endforeach
if cpp.has_member('passwd', 'pw_gecos', prefix: '#include <pwd.h>')
	config.set('UNISTDX_HAVE_GECOS', true)
endif
if config.has('UNISTDX_HAVE_BACKTRACE')
	if cpp.get_id() == 'gcc' or cpp.has_argument('-rdynamic')
		add_global_link_arguments('-rdynamic', language: 'cpp')
	endif
endif
unistdx_version = get_option('buildtype').contains('debug') ? '0.0.0' : meson.project_version()
config.set('UNISTDX_VERSION', unistdx_version)
v = unistdx_version.split('.')
config.set('UNISTDX_MAJOR', v[0])
config.set('UNISTDX_MINOR', v[1])
config.set('UNISTDX_PATCHLEVEL', v[2])
# }}}

threads = dependency('threads')
gtest = dependency('gtest', main: true)

subdir('src')
subdir('doc')
subdir('pkg')

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
