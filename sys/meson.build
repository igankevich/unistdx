configure_file(
	input: 'config.in',
	output: 'unistdx_config',
	configuration: config
)

libunistdx_src = files(
	'signal.cc',
	'system.cc',
	'file.cc',
	'process.cc',
	'bits/safe_calls.cc'
)

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
	'cmdline.hh',
	'dir.hh',
	'endpoint.hh',
	'event.hh',
	'fildesbuf.hh',
	'fildes.hh',
	'file.hh',
	'file_mutex.hh',
	'groups.hh',
	'ifaddr.hh',
	'ifaddr_list.hh',
	'log.hh',
	'network_format.hh',
	'packetstream.hh',
	'path.hh',
	'pipe.hh',
	'process.hh',
	'semaphore.hh',
	'sharedmem.hh',
	'shmembuf.hh',
	'signal.hh',
	'signal_semaphore.hh',
	'socket.hh',
	'subnet_iterator.hh',
	'system.hh',
	'users.hh',
	subdir: meson.project_name() + '/sys'
)

install_headers(
	'bits/basic_istream_iterator.hh',
	'bits/basic_ostream_iterator.hh',
	'bits/bit_count.hh',
	'bits/check.hh',
	'bits/endpoint_parse.hh',
	'ifaddrs_iterator.hh',
	'bits/safe_calls.hh',
	subdir: meson.project_name() + '/sys/bits'
)
