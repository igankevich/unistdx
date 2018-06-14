#include <unistdx/fs/file_status>

#include <unistdx/test/operator>
#include <unistdx/test/exception>

TEST(file_status, errors) {
	UNISTDX_EXPECT_ERROR(
		std::errc::no_such_file_or_directory,
		sys::file_status("non-existent-file")
	);
	sys::file_status st;
	UNISTDX_EXPECT_ERROR(
		std::errc::no_such_file_or_directory,
		st.update("non-existent-file")
	);
}

TEST(file_status, print) {
	test::stream_insert_starts_with("d", sys::file_status("src"));
}

TEST(file_status, members) {
	sys::file_status st("src");
	EXPECT_TRUE(st.is_directory());
	EXPECT_FALSE(st.is_regular());
	EXPECT_FALSE(st.is_block_device());
	EXPECT_FALSE(st.is_character_device());
	EXPECT_FALSE(st.is_socket());
	EXPECT_FALSE(st.is_pipe());
	EXPECT_FALSE(st.is_symbolic_link());
}
