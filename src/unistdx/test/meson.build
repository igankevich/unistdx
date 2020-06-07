empty_exe = executable('empty', sources: 'empty.cc')
test_config = configuration_data()
test_config.set('empty_exe_path', empty_exe.full_path())
test_config.set('UNISTDX_TEST_SANITIZE_ADDRESS', get_option('b_sanitize') == 'address')
status = cpp.run(files('unshare_test.cc'))
if status.compiled() and status.returncode() == 0
    test_config.set('UNISTDX_TEST_HAVE_UNSHARE', true)
endif
configure_file(
    input: 'config.in',
    output: 'config',
    configuration: test_config
)

install_headers(
    'print_flags',
    subdir: join_paths(meson.project_name(), 'test')
)
