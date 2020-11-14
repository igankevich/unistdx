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
subdir('system')
subdir('bits')
subdir('test')

unistdx_deps = []
if with_debug
    libunwind = dependency('libunwind')
    libdw = dependency('libdw')
    unistdx_deps += libunwind
    unistdx_deps += libdw
endif

unistdx_lib = library(
    'unistdx' + suffix,
    sources: libunistdx_src,
    version: meson.project_version(),
    install: true,
    include_directories: src,
    implicit_include_directories: false,
    dependencies: unistdx_deps,
    cpp_args: cpp_args,
    link_args: cpp_linker_args,
)

pkgconfig.generate(
    libraries: unistdx_lib,
    requires: unistdx_deps,
    version: meson.project_version(),
    name: meson.project_name() + suffix,
    description: 'C++ wrappers for libc',
    extra_cflags: cpp_args,
)

unistdx = declare_dependency(
    link_with: unistdx_lib,
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
                dependencies: [unistdx,gtest,threads] + (valgrind_dep.found() ? [valgrind_dep] : []) + (with_debug ? [libunwind,libdw] : []),
                link_with: libs,
                implicit_include_directories: false,
            )
            foreach args : arguments
                tmp = test_name + suffix
                if args.length() != 0
                    tmp += '--' + '_'.join(args)
                endif
                test(tmp, exe, args: args)
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
            dependencies: [unistdx] + (with_debug ? [libunwind,libdw] : []),
            implicit_include_directories: false,
        )
    )
endforeach

if with_site
    subdir('doxygen')
    subdir('site')
endif
