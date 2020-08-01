libunistdx_src += files([
    'copy_file.cc',
    'file_mode.cc',
    'file_mutex.cc',
    'file_status.cc',
    'file_type.cc',
    'mkdirs.cc',
    'odirtree.cc',
])

install_headers(
    'canonical_path',
    'copy_file',
    'directory',
    'directory_entry',
    'dirstream',
    'file_mode',
    'file_mutex',
    'file_status',
    'file_type',
    'idirectory',
    'idirtree',
    'mkdirs',
    'odirectory',
    'odirtree',
    'path',
    'path_flag',
    subdir: join_paths(meson.project_name(), 'fs')
)

libunistdx_tests += [
    [files('copy_file_test.cc'),
     [[],
      [copy_file_range_stub],
      [copy_file_range_stub, sendfile_stub]]],
    [files('file_mode_test.cc')],
    [files('file_mutex_test.cc')],
    [files('file_status_test.cc')],
    [files('file_type_test.cc')],
    [files('idirectory_test.cc')],
    [files('idirtree_test.cc')],
    [files('mkdirs_test.cc')],
    [files('path_test.cc')],
]
