project(
	'unistd++',
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

cpp = meson.get_compiler('cpp')
libpthread = cpp.find_library('pthread')
libstdx = dependency('stdx')

subdir('test')
