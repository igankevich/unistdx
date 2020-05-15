empty_exe = executable('empty', sources: 'empty.cc')
test_config = configuration_data()
test_config.set('empty_exe_path', empty_exe.full_path())
test_config.set('UNISTDX_TEST_SANITIZE_ADDRESS', get_option('b_sanitize') == 'address')
configure_file(
    input: 'config.in',
    output: 'config',
    configuration: test_config
)
