configure_file(
	input: 'config.in',
	output: 'unistdx_config',
	configuration: config
)

libunistdx_src = files(
	'system.cc',
	'bits/safe_calls.cc',
	'backtrace.cc',
)

subdir('fs')
subdir('net')
subdir('io')
subdir('ipc')

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
	include_directories: include_directories('.')
)

install_headers(
	'groups.hh',
	'network',
	'filesystem',
	'process',
	'system.hh',
	'users.hh',
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
