project(
	'unistdx',
	'cpp',
	version: '0.2',
	meson_version: '>=0.31',
	default_options: ['cpp_std=c++11']
)

if get_option('buildtype') == 'release'
	add_global_arguments('-DNDEBUG', language: 'cpp')
endif

srcdir = include_directories('.')
testdir = include_directories('test')

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
# }}}

threads = dependency('threads')
libstdx = dependency('stdx')
gtest = dependency('gtest', main: true)

subdir('sys')
subdir('test')

pkgconfig = import('pkgconfig')
pkgconfig.generate(
	libraries: libunistdx_shared,
	version: meson.project_version(),
	name: meson.project_name(),
	description: 'C++ wrappers for libc',
	subdirs: meson.project_name(),
	filebase: meson.project_name()
)

prefix = meson.project_name() + '-' + meson.project_version()
run_target('archive', command: [
	'git',
	'archive',
	'--format=tar.gz',
	'--output=' + meson.build_root() + '/' + prefix + '.tar.gz',
	'--prefix=' + prefix + '/',
	'HEAD'
])
