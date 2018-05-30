#include <unistdx/fs/copy_file>
#include <unistdx/fs/file_stat>
#include <unistdx/io/fildes>
#include <unistdx/test/random_string>
#include <unistdx/test/tmpdir>

std::vector<size_t> all_sizes{0, 1, 2, 111, 4096, 4097, 10000};

class copy_file_test: public ::testing::TestWithParam<size_t> {};

TEST_P(copy_file_test, all) {
	test::tmpdir tmp(__func__);
	sys::path src(tmp.name(), "x");
	sys::path dst(tmp.name(), "y");
	std::ofstream(src) << test::random_string<char>(GetParam());
	sys::copy_file(src, dst);
	std::stringstream orig;
	orig << std::ifstream(src).rdbuf();
	std::stringstream copy;
	copy << std::ifstream(dst).rdbuf();
	EXPECT_EQ(orig.str(), copy.str());
}

INSTANTIATE_TEST_CASE_P(
	all_sizes,
	copy_file_test,
	::testing::ValuesIn(all_sizes)
);

void
my_copy_file_simple(const sys::path& src, const sys::path& dest) {
	using namespace sys;
	fildes in(
		src,
		open_flag::read_only |
		open_flag::close_on_exec,
		0644
	);
	fildes out(
		dest,
		open_flag::create |
		open_flag::truncate |
		open_flag::write_only |
		open_flag::close_on_exec,
		0644
	);
	file_stat st(src);
	offset_type file_size = st.size();
	const size_t n = 4096;
	char buf[n];
	ssize_t nread = 0;
	do {
		ssize_t m = in.read(buf, n);
		nread += m;
		ssize_t nwritten = 0;
		do {
			nwritten += out.write(buf, m);
		} while (nwritten != m);
	} while (nread != file_size);
}


class copy_file_simple_test: public ::testing::TestWithParam<size_t> {};

TEST_P(copy_file_simple_test, all) {
	test::tmpdir tmp(__func__);
	sys::path src(tmp.name(), "x");
	sys::path dst(tmp.name(), "y");
	std::ofstream(src) << test::random_string<char>(GetParam());
	sys::copy_file(src, dst);
	std::stringstream orig;
	orig << std::ifstream(src).rdbuf();
	std::stringstream copy;
	copy << std::ifstream(dst).rdbuf();
	EXPECT_EQ(orig.str(), copy.str());
}

INSTANTIATE_TEST_CASE_P(
	all_sizes,
	copy_file_simple_test,
	::testing::ValuesIn(all_sizes)
);

