#include <algorithm>
#include <set>
#include <unistdx/base/make_object>

#include <unistdx/fs/canonical_path>
#include <unistdx/fs/idirectory>
#include <unistdx/fs/idirtree>
#include <unistdx/fs/odirectory>
#include <unistdx/fs/odirtree>
#include <unistdx/fs/path>
#include <unistdx/test/temporary_file>

#include <gtest/gtest.h>

#include "base.hh"

TEST(idirtree, open_close) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirtree tree;
	test::test_open_close(tree, tdir);
}

TEST(idirtree, current_dir) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	sys::idirtree tree(tdir);
	EXPECT_EQ(tree.current_dir(), static_cast<const sys::path&>(tdir));
}

TEST(idirtree, Iterator) {
	std::vector<std::string> files {"a", "b", "c"};
	std::vector<std::string> files_d {"e", "f", "g"};
	std::vector<std::string> files_h {"i", "j", "k", "l"};
	test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
	test::tmpdir tdir_d(
		sys::path(UNISTDX_TMPDIR, "d"),
		files_d.begin(),
		files_d.end()
	);
	test::tmpdir tdir_h(
		sys::path(UNISTDX_TMPDIR, "h"),
		files_h.begin(),
		files_h.end()
	);
	std::vector<std::string> all_files;
	std::copy(files.begin(), files.end(), std::back_inserter(all_files));
	std::copy(files_d.begin(), files_d.end(), std::back_inserter(all_files));
	std::copy(files_h.begin(), files_h.end(), std::back_inserter(all_files));
	all_files.emplace_back("d");
	all_files.emplace_back("h");
	typedef sys::idirtree_iterator<sys::direntry> iterator;
	test::test_file_count<sys::idirtree,iterator>(tdir, all_files);
	test::test_file_list<sys::idirtree,iterator>(tdir, all_files);
}

TEST(FileStat, Exists) {
	sys::canonical_path cwd(".");
	sys::path non_existent_file(cwd, "asdasdasd");
	sys::file_status stat;
	try {
		stat.update(non_existent_file);
	} catch (const sys::bad_call& err) {
		EXPECT_EQ(ENOENT, err.code().value());
	}
	EXPECT_FALSE(stat.exists());
}

TEST(GetFileType, DirEntry) {
	test::temporary_file tmp(UNISTDX_TMPFILE);
	std::ofstream(tmp.path()) << "hello world";
	sys::path cwd(".");
	sys::idirectory dir(cwd);
	sys::idirectory_iterator<sys::direntry> end;
	auto result =
		std::find_if(
			sys::idirectory_iterator<sys::direntry>(dir),
			end,
			[&] (const sys::direntry& rhs) {
			    return rhs.name() == tmp.path();
			}
		);
	EXPECT_NE(result, end);
	EXPECT_EQ(sys::file_type::regular, sys::get_file_type(cwd, *result));
}

TEST(GetFileType, PathEntry) {
	test::temporary_file tmp(UNISTDX_TMPFILE);
	std::ofstream(tmp.path()) << "hello world";
	sys::path cwd(".");
	sys::idirtree dir(cwd);
	sys::idirtree_iterator<sys::pathentry> end;
	auto result =
		std::find_if(
			sys::idirtree_iterator<sys::pathentry>(dir),
			end,
			[&] (const sys::pathentry& rhs) {
			    return rhs.name() == tmp.path();
			}
		);
	EXPECT_NE(result, end);
	EXPECT_EQ(sys::file_type::regular, sys::get_file_type(*result));
}

typedef std::tuple<std::string> idirtree_test_param;

struct idirtree_test: public ::testing::TestWithParam<idirtree_test_param> {};

std::vector<idirtree_test_param> all_params {
	{"dirtree-copy-in"},
	{"dirtree-copy-in/"},
	{"dirtree-copy-in//"},
};

TEST_P(idirtree_test, insert) {
	sys::path src(std::get<0>(GetParam()));
	std::vector<std::string> files1 {"a", "b", "c"};
	std::vector<std::string> files2 {"d", "e", "f"};
	test::tmpdir dir1(src, files1.begin(), files1.end());
	test::tmpdir dir2(sys::path(src, "next"), files2.begin(), files2.end());
	test::tmpdir dir3("dirtree-copy-out");
	std::set<sys::path> expected;
	expected.emplace("a");
	expected.emplace("b");
	expected.emplace("c");
	expected.emplace("next");
	expected.emplace("next/d");
	expected.emplace("next/e");
	expected.emplace("next/f");
	{
		sys::idirtree idir(dir1);
		sys::odirtree odir(dir3);
		odir.settransform(sys::copy_recursively{dir1, dir3});
		std::copy(
			sys::idirtree_iterator<sys::pathentry>(idir),
			sys::idirtree_iterator<sys::pathentry>(),
			sys::odirtree_iterator<sys::pathentry>(odir)
		);
	}
	std::set<sys::path> actual;
	{
		sys::idirtree idir(dir3);
		std::transform(
			sys::idirtree_iterator<sys::pathentry>(idir),
			sys::idirtree_iterator<sys::pathentry>(),
			std::inserter(actual, actual.begin()),
			[&] (const sys::pathentry& rhs) {
				sys::path p = rhs.getpath();
				if (p.find(dir3.name()) == 0) {
					p = p.substr(dir3.name().size()+1);
				}
				return p;
			}
		);
	}
	EXPECT_EQ(expected, actual);
}

INSTANTIATE_TEST_CASE_P(
	_,
	idirtree_test,
	::testing::ValuesIn(all_params)
);

