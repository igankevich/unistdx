#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

#include <unistdx/fs/idirectory>
#include <unistdx/fs/odirectory>

#include "base.hh"

TEST(idirectory, open_close) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirectory dir;
	test::test_open_close(dir, tdir);
}

TEST(idirectory, file_count) {
	typedef sys::idirectory_iterator<sys::direntry> iterator;
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	test::test_file_count<sys::idirectory,iterator>(tdir, files);
}

TEST(idirectory, file_list) {
	typedef sys::idirectory_iterator<sys::direntry> iterator;
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	test::test_file_list<sys::idirectory,iterator>(tdir, files);
}

TEST(idirectory, entries) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirectory dir(tdir);
	std::set<std::string> actual, expected(files.begin(), files.end());
	for (const auto& entry : dir.entries<sys::direntry>()) {
		actual.insert(entry.name());
	}
	EXPECT_EQ(expected, actual);
}

TEST(idirectory, insert) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirectory dir(tdir);
	test::tmpdir otdir(UNISTDX_TMPDIR_OUT);
	sys::odirectory odir(otdir);
	std::copy(
		sys::idirectory_iterator<sys::pathentry>(dir),
		sys::idirectory_iterator<sys::pathentry>(),
		sys::odirectory_iterator<sys::pathentry>(odir)
	);
	std::set<sys::direntry> orig;
	{
		sys::idirectory idir1(tdir);
		std::copy(
			sys::idirectory_iterator<sys::direntry>(idir1),
			sys::idirectory_iterator<sys::direntry>(),
			std::inserter(orig, orig.end())
		);
	}
	std::set<sys::direntry> copied;
	{
		sys::idirectory idir2(otdir);
		std::copy(
			sys::idirectory_iterator<sys::direntry>(idir2),
			sys::idirectory_iterator<sys::direntry>(),
			std::inserter(copied, copied.end())
		);
	}
	EXPECT_EQ(orig, copied)
	    << "bad file copy from " << tdir.name()
	    << " to " << otdir.name();
}

