configure_file(
	input: 'config.in',
	output: 'unistdx_config',
	configuration: config
)

libunistdx_src = files(
	'bits/safe_calls.cc',
)

subdir('fs')
subdir('net')
subdir('io')
subdir('ipc')
subdir('util')

libunistdx_shared = shared_library(
	'unistdx',
	sources: libunistdx_src,
	install: true,
	include_directories: srcdir
)

libunistdx_static = static_library(
	'unistdx',
	sources: libunistdx_src,
	install: true,
	include_directories: srcdir
)

unistdx = declare_dependency(
	link_with: libunistdx_static,
	include_directories: srcdir
)

install_headers(
	'network',
	'filesystem',
	'process',
	'utils',
	'fildes',
	subdir: meson.project_name() + '/sys'
)

install_headers(
	'bits/basic_istream_iterator.hh',
	'bits/basic_ostream_iterator.hh',
	'bits/bit_count.hh',
	'check',
	'bits/endpoint_parse.hh',
	'bits/safe_calls.hh',
	subdir: meson.project_name() + '/sys/bits'
)
