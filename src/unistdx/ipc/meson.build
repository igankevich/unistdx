libunistdx_src += files([
    'identity.cc',
    'process.cc',
    'process_group.cc',
    'process_status.cc',
    'signal.cc',
])

install_headers(
    'argstream',
    'execute',
    'identity',
    'process',
    'process_group',
    'process_semaphore',
    'process_status',
    'semaphore',
    'shared_memory_segment',
    'shmembuf',
    'signal',
    'thread_semaphore',
    subdir: join_paths(meson.project_name(), 'ipc')
)

libunistdx_tests += [
    [files([
        'execute_test.cc',
        'process_test.cc',
        'process_group_test.cc',
        'process_status_test.cc',
        'semaphore_test.cc',
        'shared_memory_segment_test.cc',
        'signal_test.cc',
    ])],
    [files('identity_test.cc'), [[], [setgid_stub]]],
]

if host_machine.cpu() != 'aarch64'
   libunistdx_tests += [files('sysv_semaphore_test.cc')]
endif
