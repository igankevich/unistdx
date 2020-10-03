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
    [files('execute_test.cc')],
    [files('identity_test.cc'), [[], [setgid_stub]]],
    [files('process_test.cc')],
    [files('process_group_test.cc')],
    [files('process_status_test.cc')],
    [files('semaphore_test.cc')],
    [files('sysv_semaphore_test.cc')],
    [files('shared_memory_segment_test.cc')],
    [files('signal_test.cc')],
]
