configure_file(
	input: 'config.in',
	output: 'unistdx_config',
	configuration: config
)

libunistdx_src = files(
	'signal.cc',
	'system.cc',
	'process.cc',
	'bits/safe_calls.cc',
	'backtrace.cc',
	'pipe.cc'
)

subdir('fs')
subdir('net')

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
	'argstream.hh',
	'poller',
	'fildesbuf.hh',
	'fildes.hh',
	'groups.hh',
	'network',
	'packetstream.hh',
	'pipe.hh',
	'filesystem',
	'process.hh',
	'semaphore.hh',
	'sharedmem',
	'shmembuf.hh',
	'signal',
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
