#include <unistdx/fs/file_stat>

#include <unistdx/test/operator>
#include <unistdx/test/exception>

TEST(file_stat, errors) {
	UNISTDX_EXPECT_ERROR(
		std::errc::no_such_file_or_directory,
		sys::file_stat("non-existent-file")
	);
	sys::file_stat st;
	UNISTDX_EXPECT_ERROR(
		std::errc::no_such_file_or_directory,
		st.update("non-existent-file")
	);
}

TEST(file_stat, print) {
	test::stream_insert_starts_with("d", sys::file_stat("src"));
}

TEST(file_stat, members) {
	sys::file_stat st("src");
	EXPECT_TRUE(st.is_directory());
	EXPECT_FALSE(st.is_regular());
	EXPECT_FALSE(st.is_block_device());
	EXPECT_FALSE(st.is_character_device());
	EXPECT_FALSE(st.is_socket());
	EXPECT_FALSE(st.is_pipe());
	EXPECT_FALSE(st.is_symbolic_link());
}
