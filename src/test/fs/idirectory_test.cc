#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

#include <unistdx/fs/idirectory>
#include <unistdx/fs/odirectory>

#include <unistdx/test/operator>

#include "base.hh"

TEST(idirectory, open_close) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirectory dir;
	test::test_open_close(dir, tdir);
}

TEST(idirectory, file_count) {
	typedef sys::idirectory_iterator<sys::directory_entry> iterator;
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	test::test_file_count<sys::idirectory,iterator>(tdir, files);
}

TEST(idirectory, file_list) {
	typedef sys::idirectory_iterator<sys::directory_entry> iterator;
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	test::test_file_list<sys::idirectory,iterator>(tdir, files);
}

TEST(idirectory, entries) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirectory dir(tdir);
	std::set<std::string> actual, expected(files.begin(), files.end());
	for (const auto& entry : dir.entries<sys::directory_entry>()) {
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
	std::set<sys::directory_entry> orig;
	{
		sys::idirectory idir1(tdir);
		std::copy(
			sys::idirectory_iterator<sys::directory_entry>(idir1),
			sys::idirectory_iterator<sys::directory_entry>(),
			std::inserter(orig, orig.end())
		);
	}
	std::set<sys::directory_entry> copied;
	{
		sys::idirectory idir2(otdir);
		std::copy(
			sys::idirectory_iterator<sys::directory_entry>(idir2),
			sys::idirectory_iterator<sys::directory_entry>(),
			std::inserter(copied, copied.end())
		);
	}
	EXPECT_EQ(orig, copied)
	    << "bad file copy from " << tdir.name()
	    << " to " << otdir.name();
}

TEST(directory_entry, members) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirectory dir(tdir);
	sys::directory_entry ent1, ent2, ent3;
	EXPECT_TRUE(static_cast<bool>(dir));
	EXPECT_TRUE(dir.is_open());
	dir >> ent1;
	EXPECT_TRUE(static_cast<bool>(dir));
	EXPECT_EQ(ent1.name(), test::stream_insert(ent1));
	EXPECT_EQ(sys::file_type::regular, sys::get_file_type(dir.getpath(), ent1));
	EXPECT_NE(0, ent1.inode());
	EXPECT_FALSE(ent1.is_parent_dir());
	EXPECT_FALSE(ent1.is_working_dir());
	dir >> ent2;
	EXPECT_TRUE(static_cast<bool>(dir));
	EXPECT_EQ(ent2.name(), test::stream_insert(ent2));
	EXPECT_EQ(sys::file_type::regular, sys::get_file_type(dir.getpath(), ent2));
	EXPECT_STRNE(ent1.name(), ent2.name());
	EXPECT_NE(ent1, ent2);
	ent3 = ent1;
	EXPECT_EQ(ent1, ent3);
	sys::directory_entry ent4(ent2);
	EXPECT_EQ(ent2, ent4);
	sys::directory_entry ent5(std::move(ent4));
	EXPECT_EQ(ent5, ent4);
}

