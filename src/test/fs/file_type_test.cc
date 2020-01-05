#include <unistdx/fs/file_type>

#include <unistdx/test/operator>

TEST(file_type, print) {
    test::stream_insert_equals("-", sys::file_type::regular);
    test::stream_insert_equals("d", sys::file_type::directory);
    test::stream_insert_equals("b", sys::file_type::block_device);
    test::stream_insert_equals("c", sys::file_type::character_device);
    test::stream_insert_equals("p", sys::file_type::pipe);
    test::stream_insert_equals("s", sys::file_type::socket);
    test::stream_insert_equals("l", sys::file_type::symbolic_link);
    test::stream_insert_equals("?", sys::file_type(0));
}
