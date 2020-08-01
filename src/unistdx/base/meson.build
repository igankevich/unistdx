libunistdx_src += files([
    'bad_call.cc',
    'base64.cc',
    'byte_buffer.cc',
    'command_line.cc',
    'sha1.cc',
    'uint128.cc',
    'websocketbuf.cc',
    'websocket.cc',
])

install_headers(
    'bad_call',
    'base64',
    'byte_buffer',
    'check',
    'command_line',
    'contracts',
    'delete_each',
    'flag',
    'ios_guard',
    'log_message',
    'make_object',
    'packetbuf',
    'recursive_spin_mutex',
    'sha1',
    'simple_lock',
    'spin_mutex',
    'streambuf_traits',
    'types',
    'uint128',
    'unlock_guard',
    'websocket',
    'websocketbuf',
    subdir: join_paths(meson.project_name(), 'base')
)

libunistdx_tests += [
    [files('bad_call_test.cc')],
    [files('byte_buffer_test.cc')],
    [files('command_line_test.cc')],
    [files('delete_each_test.cc')],
    [files('flag_test.cc')],
    [files('log_message_test.cc')],
    [files('sha1_test.cc')],
    [files('spin_mutex_test.cc')],
    [files('uint128_test.cc')],
    [files('websocket_test.cc')],
]

libunistdx_benchmarks += files([
    'sha1_benchmark.cc',
])
