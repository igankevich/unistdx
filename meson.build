project(
    'unistdx',
    'cpp',
    version: '0.32.0',
    meson_version: '>=0.47',
    default_options: ['cpp_std=c++11'],
    license: ['Public domain']
)

with_tests = get_option('with_tests')
with_rpm = get_option('with_rpm')
with_site = get_option('with_site')
with_rpm = get_option('with_rpm')
with_debug = get_option('with_debug')

cpp = meson.get_compiler('cpp')

if with_debug
    add_global_arguments('-DUNISTDX_DEBUG', language: 'cpp')
endif

foreach arg : [
    '-Werror=return-type',
    '-Werror=return-local-addr',
    '-Werror=cast-align',
    '-Wcast-align=strict']
    if cpp.has_argument(arg)
        add_global_arguments(arg, language: 'cpp')
    endif
endforeach

# compile-time configuration {{{
all_defines = []
config = configuration_data()
foreach pair : [
    ['bswap16', '0'],
    ['bswap32', '0'],
    ['bswap64', '0'],
    ['clz', '1'],
    ['clzl', '1'],
    ['clzll', '1'],
    ['popcount', '0'],
    ['popcountl', '0'],
    ['popcountll', '0'],
    ['unreachable', ''],
]
    func = pair[0]
    args = pair[1]
    builtin = 'int main() { @0@(@1@); }'
    all_defines += 'UNISTDX_HAVE_BUILTIN_' + func.to_upper()
    if cpp.compiles(builtin.format('__builtin_' + func, args))
        config.set(all_defines[-1], true)
    endif
endforeach
foreach flag : [
    '-Wterminate',
]
    builtin = 'int main() { _Pragma("GCC diagnostic ignored \"@0@\""); }'
    all_defines += 'UNISTDX_HAVE_PRAGMA_IGNORED' + flag.underscorify().to_upper()
    if cpp.compiles(builtin.format(flag), args: '-Werror')
        config.set(all_defines[-1], true)
    endif
endforeach
foreach pragma : [
    'GCC diagnostic push',
    'GCC diagnostic pop',
]
    builtin = 'int main() { _Pragma("@0@"); }'
    all_defines += 'UNISTDX_HAVE_PRAGMA_' + pragma.underscorify().to_upper()
    if cpp.compiles(builtin.format(flag), args: '-Werror')
        config.set(all_defines[-1], true)
    endif
endforeach
if host_machine.endian() == 'little'
    all_defines += 'UNISTDX_LITTLE_ENDIAN'
    config.set(all_defines[-1], true)
elif host_machine.endian() == 'big'
    all_defines += 'UNISTDX_BIG_ENDIAN'
    config.set(all_defines[-1], true)
else
    error('unknown endiannes')
endif
foreach pair : [
    ['__int128', 'int128'],
    ['long double'],
    ['long long'],
]
    type = pair[0]
    name = type.underscorify()
    if pair.length() > 1
        name = pair[1]
    endif
    all_defines += 'UNISTDX_HAVE_' + name.to_upper()
    if cpp.has_type(type)
        config.set(all_defines[-1], true)
        config.set('UNISTDX_SIZE_OF_' + name.to_upper(), cpp.sizeof(type))
    endif
endforeach
foreach pair : [
    ['dirent.h', 'DTTOIF'],
    ['execinfo.h', 'backtrace'],
    ['fcntl.h', 'F_GETPIPE_SZ'],
    ['fcntl.h', 'F_SETNOSIGPIPE'],
    ['fcntl.h', 'F_SETPIPE_SZ'],
    ['fcntl.h', 'O_CLOEXEC'],
    ['fcntl.h', 'O_DIRECT'],
    ['fcntl.h', 'O_DIRECTORY'],
    ['fcntl.h', 'O_LARGEFILE'],
    ['fcntl.h', 'O_NOATIME'],
    ['fcntl.h', 'O_NOFOLLOW'],
    ['fcntl.h', 'O_PATH'],
    ['fcntl.h', 'O_RSYNC'],
    ['fcntl.h', 'O_TMPFILE'],
    ['fcntl.h', 'splice'],
    ['fcntl.h', 'SPLICE_F_GIFT'],
    ['fcntl.h', 'SPLICE_F_MORE'],
    ['fcntl.h', 'SPLICE_F_MOVE'],
    ['fcntl.h', 'SPLICE_F_NONBLOCK'],
    ['grp.h', 'getgrgid_r'],
    ['grp.h', 'getgrnam_r'],
    ['linux/sockios.h', 'SIOCBRADDBR'],
    ['linux/sockios.h', 'SIOCBRADDIF'],
    ['linux/sockios.h', 'SIOCBRDELBR'],
    ['linux/sockios.h', 'SIOCBRDELIF'],
    ['net/if.h', 'IFF_ALLMULTI'],
    ['net/if.h', 'IFF_BROADCAST'],
    ['net/if.h', 'IFF_DEBUG'],
    ['net/if.h', 'IFF_LOOPBACK'],
    ['net/if.h', 'IFF_MASTER'],
    ['net/if.h', 'IFF_MULTICAST'],
    ['net/if.h', 'IFF_NOARP'],
    ['net/if.h', 'IFF_NOTRAILERS'],
    ['net/if.h', 'IFF_POINTOPOINT'],
    ['net/if.h', 'IFF_PORTSEL'],
    ['net/if.h', 'IFF_PROMISC'],
    ['net/if.h', 'IFF_RUNNING'],
    ['net/if.h', 'IFF_SLAVE'],
    ['net/if.h', 'IFF_UP'],
    ['netinet/tcp.h', 'TCP_USER_TIMEOUT'],
    ['poll.h', 'POLLRDHUP'],
    ['pwd.h', 'getpwnam_r'],
    ['pwd.h', 'getpwuid_r'],
    ['sched.h', 'clone'],
    ['sched.h', 'CLONE_FILES'],
    ['sched.h', 'CLONE_FS'],
    ['sched.h', 'CLONE_NEWCGROUP'],
    ['sched.h', 'CLONE_NEWIPC'],
    ['sched.h', 'CLONE_NEWNET'],
    ['sched.h', 'CLONE_NEWNS'],
    ['sched.h', 'CLONE_NEWPID'],
    ['sched.h', 'CLONE_NEWUSER'],
    ['sched.h', 'CLONE_NEWUTS'],
    ['sched.h', 'CLONE_SYSVSEM'],
    ['sched.h', 'setns'],
    ['sched.h', 'unshare'],
    ['semaphore.h', 'sem_timedwait'],
    ['signal.h', 'SIGPOLL'],
    ['signal.h', 'SIGPWR'],
    ['signal.h', 'SIGSTKFLT'],
    ['signal.h', 'SIGWINCH'],
    ['stdlib.h', 'mkstemp'],
    ['stdlib.h', 'mkostemp'],
    ['sys/ioctl.h', 'FIONREAD'],
    ['sys/ioctl.h', 'ioctl'],
    ['sys/ioctl.h', 'SIOCADDMULTI'],
    ['sys/ioctl.h', 'SIOCDELMULTI'],
    ['sys/ioctl.h', 'SIOCDIFADDR'],
    ['sys/ioctl.h', 'SIOCGIFADDR'],
    ['sys/ioctl.h', 'SIOCGIFBRDADDR'],
    ['sys/ioctl.h', 'SIOCGIFCONF'],
    ['sys/ioctl.h', 'SIOCGIFCOUNT'],
    ['sys/ioctl.h', 'SIOCGIFDSTADDR'],
    ['sys/ioctl.h', 'SIOCGIFENCAP'],
    ['sys/ioctl.h', 'SIOCGIFFLAGS'],
    ['sys/ioctl.h', 'SIOCGIFFLAGS'],
    ['sys/ioctl.h', 'SIOCGIFHWADDR'],
    ['sys/ioctl.h', 'SIOCGIFINDEX'],
    ['sys/ioctl.h', 'SIOCGIFMEM'],
    ['sys/ioctl.h', 'SIOCGIFMETRIC'],
    ['sys/ioctl.h', 'SIOCGIFMTU'],
    ['sys/ioctl.h', 'SIOCGIFNAME'],
    ['sys/ioctl.h', 'SIOCGIFNETMASK'],
    ['sys/ioctl.h', 'SIOCGIFPFLAGS'],
    ['sys/ioctl.h', 'SIOCGIFSLAVE'],
    ['sys/ioctl.h', 'SIOCSIFADDR'],
    ['sys/ioctl.h', 'SIOCSIFBRDADDR'],
    ['sys/ioctl.h', 'SIOCSIFDSTADDR'],
    ['sys/ioctl.h', 'SIOCSIFENCAP'],
    ['sys/ioctl.h', 'SIOCSIFFLAGS'],
    ['sys/ioctl.h', 'SIOCSIFFLAGS'],
    ['sys/ioctl.h', 'SIOCSIFHWADDR'],
    ['sys/ioctl.h', 'SIOCSIFHWBROADCAST'],
    ['sys/ioctl.h', 'SIOCSIFLINK'],
    ['sys/ioctl.h', 'SIOCSIFMEM'],
    ['sys/ioctl.h', 'SIOCSIFMETRIC'],
    ['sys/ioctl.h', 'SIOCSIFMTU'],
    ['sys/ioctl.h', 'SIOCSIFNAME'],
    ['sys/ioctl.h', 'SIOCSIFNETMASK'],
    ['sys/ioctl.h', 'SIOCSIFPFLAGS'],
    ['sys/ioctl.h', 'SIOCSIFSLAVE'],
    ['sys/mman.h', 'MADV_DODUMP'],
    ['sys/mman.h', 'MADV_DOFORK'],
    ['sys/mman.h', 'MADV_DONTDUMP'],
    ['sys/mman.h', 'MADV_DONTFORK'],
    ['sys/mman.h', 'MADV_FREE'],
    ['sys/mman.h', 'MADV_HUGEPAGE'],
    ['sys/mman.h', 'MADV_HWPOISON'],
    ['sys/mman.h', 'MADV_KEEPONFORK'],
    ['sys/mman.h', 'MADV_MERGEABLE'],
    ['sys/mman.h', 'MADV_NOHUGEPAGE'],
    ['sys/mman.h', 'MADV_REMOVE'],
    ['sys/mman.h', 'MADV_SOFT_OFFLINE'],
    ['sys/mman.h', 'MADV_UNMERGEABLE'],
    ['sys/mman.h', 'MADV_WIPEONFORK'],
    ['sys/mman.h', 'MAP_32BIT'],
    ['sys/mman.h', 'MAP_ANONYMOUS'],
    ['sys/mman.h', 'MAP_GROWSDOWN'],
    ['sys/mman.h', 'MAP_HUGE_1GB'],
    ['sys/mman.h', 'MAP_HUGE_2MB'],
    ['sys/mman.h', 'MAP_HUGETLB'],
    ['sys/mman.h', 'MAP_LOCKED'],
    ['sys/mman.h', 'MAP_NONBLOCK'],
    ['sys/mman.h', 'MAP_NORESERVE'],
    ['sys/mman.h', 'MAP_POPULATE'],
    ['sys/mman.h', 'MAP_STACK'],
    ['sys/mman.h', 'MAP_UNINITIALIZED'],
    ['sys/mman.h', 'mmap'],
    ['sys/prctl.h', 'prctl'],
    ['sys/prctl.h', 'PR_GET_NO_NEW_PRIVS'],
    ['sys/prctl.h', 'PR_SET_NO_NEW_PRIVS'],
    ['sys/sem.h', 'semtimedop'],
    ['sys/sendfile.h', 'sendfile'],
    ['sys/shm.h', 'SHM_HUGE_1GB'],
    ['sys/shm.h', 'SHM_HUGE_2MB'],
    ['sys/shm.h', 'SHM_HUGE_SHIFT'],
    ['sys/shm.h', 'SHM_HUGETLB'],
    ['sys/shm.h', 'SHM_NORESERVE'],
    ['sys/socket.h', 'accept4'],
    ['sys/socket.h', 'SCM_CREDENTIALS'],
    ['sys/socket.h', 'SCM_RIGHTS'],
    ['sys/socket.h', 'SOCK_CLOEXEC'],
    ['sys/socket.h', 'SOCK_NONBLOCK'],
    ['sys/socket.h', 'SO_PEERCRED'],
    ['unistd.h', 'copy_file_range'],
    ['unistd.h', 'pipe2'],
    ['unistd.h', 'SEEK_DATA'],
    ['unistd.h', 'SEEK_HOLE'],
]
    header = pair[0]
    symbol = pair[1]
    all_defines += 'UNISTDX_HAVE_' + symbol.to_upper()
    if cpp.has_header_symbol(header, symbol)
        config.set(all_defines[-1], true)
    endif
endforeach
foreach header : [
    'cxxabi.h',
    'ifaddrs.h',
    'linux/netlink.h',
    'linux/sockios.h',
    'netdb.h',
    'netinet/tcp.h',
    'sched.h',
    'semaphore.h',
    'sys/epoll.h',
    'sys/ioctl.h',
    'sys/resource.h',
    'sys/sem.h',
    'sys/time.h',
    'sys/xattr.h',
]
    all_defines += 'UNISTDX_HAVE_' + header.underscorify().to_upper()
    if cpp.has_header(header)
        config.set(all_defines[-1], true)
    endif
endforeach
all_defines += 'UNISTDX_HAVE_GECOS'
if cpp.has_member('passwd', 'pw_gecos', prefix: '#include <pwd.h>')
    config.set('UNISTDX_HAVE_GECOS', true)
endif
if config.has('UNISTDX_HAVE_BACKTRACE')
    if cpp.get_id() == 'gcc' or cpp.has_link_argument('-rdynamic')
        add_global_link_arguments('-rdynamic', language: 'cpp')
    endif
endif
unistdx_version = get_option('buildtype').contains('debug') ? '0.0.0' : meson.project_version()
config.set('UNISTDX_VERSION', unistdx_version)
v = unistdx_version.split('.')
config.set('UNISTDX_MAJOR', v[0])
config.set('UNISTDX_MINOR', v[1])
config.set('UNISTDX_PATCHLEVEL', v[2])
# }}}

threads = dependency('threads')
if with_tests
    gtest = dependency('gtest', main: true)
endif

subdir('src')
if with_rpm
    subdir('rpm')
endif

# pkgconfig {{{
pkgconfig = import('pkgconfig')
pkgconfig.generate(
    libraries: libunistdx_shared,
    version: meson.project_version(),
    name: meson.project_name(),
    description: 'C++ wrappers for libc',
    filebase: meson.project_name()
)
# }}}
# cppcheck {{{
cppcheck = find_program('cppcheck', required: false)
if cppcheck.found()
    run_target(
        'cppcheck',
        command: [
            cppcheck,
            '--enable=all',
            '--quiet',
            '--force',
            '--language=c++',
            '--std=c++11',
            '--template=gcc',
            '-I' + join_paths(meson.source_root(), 'src'),
            '-I' + join_paths(meson.build_root(), 'src'),
            join_paths(meson.source_root(), 'src'),
        ]
    )
endif
# }}}
# clang-check {{{
clang_check = find_program('clang-check', required: false)
if clang_check.found()
    run_target(
        'clang-check',
        command: [clang_check, '-analyze'] + libunistdx_src + [
            '--',
            '-I' + join_paths(meson.source_root(), 'src'),
            '-I' + join_paths(meson.build_root(), 'src'),
            '-std=c++11'
        ]
    )
endif
# }}}
# clang-tidy {{{
clang_tidy = find_program('clang-tidy', required: false)
guix = find_program('guix', required: false)
if clang_tidy.found() and guix.found()
    clang_tidy_checks = [
        '-*',
        'bugprone-*',
        '-bugprone-unused-raii',
        '-bugprone-narrowing-conversions',
        'cppcoreguidelines-avoid-goto',
        'cppcoreguidelines-slicing',
        'cppcoreguidelines-special-member-functions',
        'misc-*',
        '-misc-non-private-member-variables-in-classes',
        'performance-*',
        '-performance-unnecessary-value-param',
    ]
    run_target(
        'clang-tidy',
        command: [
            guix,
            'repl',
            '--',
            join_paths(meson.source_root(), 'ci', 'clang-tidy.scm'),
            clang_tidy.path(),
            '-p=' + meson.build_root(),
            '--checks=' + ','.join(clang_tidy_checks),
            '--warnings-as-errors=*',
            '--quiet',
        ] + libunistdx_src + [
            '--',
            '-I' + join_paths(meson.source_root(), 'src'),
            '-I' + join_paths(meson.build_root(), 'src'),
            '-std=c++11'
        ]
    )
endif
# }}}
