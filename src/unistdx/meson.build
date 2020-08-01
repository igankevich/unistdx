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
libunistdx_tests = files()
libunistdx_benchmarks = files()

subdir('stub')
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
    include_directories: src,
    implicit_include_directories: false,
)

unistdx = declare_dependency(
    link_with: libunistdx_shared,
    include_directories: src
)

if with_tests
    foreach tuple : libunistdx_tests
        file = tuple[0]
        stubs = tuple.length() == 1 ? [[]] : tuple[1]
        arguments = tuple.length() <= 2 ? [[]] : tuple[2]
        path = '@0@'.format(file)
        basename = path.split('/')[-1]
        dirname = path.split('/')[-2]
        words = []
        foreach w : basename.split('.')[0].split('_')
            if w != 'test'
                words += w
            endif
        endforeach
        name = '_'.join(words)
        test_name = dirname + '/' + name
        source_name = file
        foreach libs : stubs
            suffix = ''
            foreach l : libs
                tmp = l.full_path().split('lib')
                suffix += '--' + tmp[tmp.length()-1].split('.so')[0]
            endforeach
            exe = executable(
                '_'.join(words) + '_test' + suffix.underscorify(),
                sources: source_name,
                include_directories: src,
                dependencies: [unistdx,gtest,threads],
                link_with: libs,
                implicit_include_directories: false,
            )
            foreach args : arguments
                test(
                    test_name + suffix + '--' + '_'.join(args),
                    exe,
                    args: args,
                )
            endforeach
        endforeach
    endforeach
endif

foreach file : libunistdx_benchmarks
    path = '@0@'.format(file)
    basename = path.split('/')[-1]
    dirname = path.split('/')[-2]
    words = []
    foreach w : basename.split('.')[0].split('_')
        if w != 'benchmark'
            words += w
        endif
    endforeach
    name = '_'.join(words)
    benchmark_name = dirname + '/' + name
    source_name = file
    benchmark(
        benchmark_name,
        executable(
            '_'.join(words) + '_benchmark',
            sources: source_name,
            include_directories: src,
            dependencies: [unistdx],
            implicit_include_directories: false,
        )
    )
endforeach

if with_site
    subdir('doxygen')
    subdir('site')
endif
