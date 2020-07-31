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

install_headers('forward', subdir: meson.project_name())

libunistdx_src = files()

subdir('base')
subdir('fs')
subdir('it')
subdir('net')
subdir('io')
subdir('ipc')
subdir('util')
subdir('bits')
subdir('test')

libunistdx_shared = shared_library(
    'unistdx',
    sources: libunistdx_src,
    version: meson.project_version(),
    install: true,
    include_directories: srcdir,
    implicit_include_directories: false,
)

unistdx = declare_dependency(
    link_with: libunistdx_shared,
    include_directories: srcdir
)

if with_site
    subdir('doxygen')
    subdir('site')
endif
