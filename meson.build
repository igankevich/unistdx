project(
	'unistdx',
	'cpp',
	version: '3.1',
	meson_version: '>=0.31',
	default_options: ['cpp_std=c++11']
)

if get_option('buildtype') == 'release'
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
if cpp.has_header_symbol('sys/socket.h', 'SOCK_NONBLOCK')
	config.set('UNISTDX_HAVE_SOCK_NONBLOCK', true)
endif
if cpp.has_header_symbol('sys/socket.h', 'SOCK_CLOEXEC')
	config.set('UNISTDX_HAVE_SOCK_CLOEXEC', true)
endif
if cpp.has_header_symbol('fcntl.h', 'F_SETNOSIGPIPE')
	config.set('UNISTDX_HAVE_SETNOSIGPIPE', true)
endif
if cpp.has_header_symbol('fcntl.h', 'O_CLOEXEC')
	config.set('UNISTDX_HAVE_O_CLOEXEC', true)
endif
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
if cpp.has_header_symbol('pwd.h', 'getpwnam_r')
	config.set('UNISTDX_HAVE_GETPWNAM_R', true)
endif
if cpp.has_header_symbol('pwd.h', 'getpwuid_r')
	config.set('UNISTDX_HAVE_GETPWUID_R', true)
endif
if cpp.has_header_symbol('grp.h', 'getgrnam_r')
	config.set('UNISTDX_HAVE_GETGRNAM_R', true)
endif
if cpp.has_header_symbol('grp.h', 'getgrgid_r')
	config.set('UNISTDX_HAVE_GETGRGID_R', true)
endif
if cpp.has_argument('-Wall')
	add_global_arguments('-Wall', language: 'cpp')
endif
if cpp.has_argument('-Wextra')
	add_global_arguments('-Wextra', language: 'cpp')
endif
# }}}

threads = dependency('threads')
gtest = dependency('gtest', main: true)

subdir('src')

pkgconfig = import('pkgconfig')
pkgconfig.generate(
	libraries: libunistdx_shared,
	version: meson.project_version(),
	name: meson.project_name(),
	description: 'C++ wrappers for libc',
	filebase: meson.project_name()
)

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
