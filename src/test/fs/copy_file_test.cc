#include <unistdx/fs/copy_file>
#include <unistdx/fs/file_stat>
#include <unistdx/io/fildes>

#include <unistdx/test/random_string>
#include <unistdx/test/temporary_file>
#include <unistdx/test/tmpdir>

std::vector<size_t> all_sizes{0, 1, 2/*, 111, 4096, 4097, 10000*/};

class copy_file_test: public ::testing::TestWithParam<size_t> {};

TEST_P(copy_file_test, all) {
	test::tmpdir tmp(UNISTDX_TMPDIR);
	std::clog << "tmp=" << tmp.name() << std::endl;
	sys::path src(tmp.name(), "x");
	sys::path dst(tmp.name(), "y");
	std::ofstream(src) << test::random_string<char>(GetParam());
	sys::copy_file(src, dst);
	std::stringstream orig;
	orig << std::ifstream(src).rdbuf();
	std::stringstream copy;
	copy << std::ifstream(dst).rdbuf();
	EXPECT_EQ(orig.str(), copy.str());
	sys::remove(src);
	sys::remove(dst);
}

INSTANTIATE_TEST_CASE_P(
	all_sizes,
	copy_file_test,
	::testing::ValuesIn(all_sizes)
);
