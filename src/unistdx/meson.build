configure_file(
	input: 'config.in',
	output: 'config',
	configuration: config,
	install_dir: join_paths(
		get_option('prefix'),
		get_option('includedir'),
		meson.project_name()
	)
)

libunistdx_src = files()

subdir('base')
subdir('fs')
subdir('it')
subdir('net')
subdir('io')
subdir('ipc')
subdir('util')
subdir('bits')

libunistdx_shared = shared_library(
	'unistdx',
	sources: libunistdx_src,
	version: meson.project_version(),
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
	'check',
	'fildes',
	subdir: join_paths(meson.project_name())
)
