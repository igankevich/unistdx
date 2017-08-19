#include <algorithm>
#include <fstream>
#include <set>
#include <unistdx/base/make_object>

#include <gtest/gtest.h>
#include <unistdx/ipc/argstream>
#include <unistdx/filesystem>

const char*
current_test_name() {
	return ::testing::UnitTest::GetInstance()->current_test_info()->name();
}

namespace test {

	template <class It>
	void
	generate_files(sys::path subdir, It first, It last) {
		std::ofstream str;
		while (first != last) {
			str.open(sys::path(subdir, *first));
			str.close();
			str.clear();
			++first;
		}
	}

	class tmpdir {

		sys::path _dirname;

	public:

		tmpdir():
		tmpdir(current_test_name()) {}

		explicit
		tmpdir(const char* dirname):
		tmpdir(sys::path(dirname)) {}

		explicit
		tmpdir(sys::path dirname):
		_dirname(dirname) {
			mkdir();
		}

		template <class It>
		tmpdir(It first, It last):
		tmpdir(current_test_name(), first, last) {}

		template <class It>
		tmpdir(const char* dirname, It first, It last):
		tmpdir(sys::path(dirname), first, last) {}

		template <class It>
		tmpdir(sys::path dirname, It first, It last):
		_dirname(dirname) {
			mkdir();
			generate_files(_dirname, first, last);
		}

		~tmpdir() {
			std::stringstream cmd;
			cmd << "rm -rf '";
			for (char ch : _dirname) {
				if (ch == '\'') {
					cmd.put('\\');
				}
				cmd.put(ch);
			}
			cmd.put('\'');
			(void)std::system(cmd.str().data());
		}

		void
		list() {
			std::stringstream cmd;
			cmd << "find " << name();
			(void)std::system(cmd.str().data());
		}

		operator const sys::path&() const noexcept {
			return _dirname;
		}

		const sys::path&
		name() const noexcept {
			return _dirname;
		}

	private:

		void
		mkdir() {
			::mkdir(_dirname, 0755);
		}

	};

}

void
assert_good(const sys::idirectory& dir, bool is_open) {
	EXPECT_EQ(dir.is_open(), is_open);
	EXPECT_EQ(dir.good(), true);
	EXPECT_EQ(dir.bad(), false);
	EXPECT_EQ(dir.eof(), false);
	EXPECT_EQ(static_cast<bool>(dir), true);
}

template <class Stream>
void
test_open_close(Stream& dir, const test::tmpdir& tdir) {
	assert_good(dir, false);
	dir.open(tdir);
	assert_good(dir, true);
	dir.close();
	assert_good(dir, false);
}

template <class Stream = sys::idirectory, class Iterator =
			  sys::idirectory_iterator<sys::direntry> >
void
test_file_count(
	const test::tmpdir& tdir,
	const
	std::vector<std::string>& files
) {
	Stream dir(tdir);
	size_t cnt = std::distance(Iterator(dir), Iterator());
	EXPECT_EQ(cnt, files.size()) << "bad total file count in " << tdir.name();
}

template <class Stream = sys::idirectory, class Iterator =
			  sys::idirectory_iterator<sys::direntry> >
void
test_file_list(
	const test::tmpdir& tdir,
	const std::vector<std::string>& files
) {
	Stream dir(tdir);
	std::set<std::string> files_orig(files.begin(), files.end());
	std::set<std::string> files_actual;
	std::for_each(
		Iterator(dir),
		Iterator(),
		[&files_actual] (const sys::direntry& ent) {
		    files_actual.emplace(ent.name());
		}
	);
	EXPECT_EQ(files_orig, files_actual) << "bad file list in " << tdir.name();
}

template <
	class IStream = sys::idirectory,
	class OStream = sys::odirectory,
	class Iterator = sys::idirectory_iterator<sys::direntry>
>
void
test_file_copy(const test::tmpdir& tdir) {
	IStream dir(tdir);
	test::tmpdir otdir(__func__);
	OStream odir(otdir);
	std::copy(
		sys::idirectory_iterator<sys::pathentry>(dir),
		sys::idirectory_iterator<sys::pathentry>(),
		sys::odirectory_iterator<sys::pathentry>(odir)
	);
	std::set<sys::direntry> orig;
	{
		IStream idir1(tdir);
		std::copy(
			sys::idirectory_iterator<sys::direntry>(idir1),
			sys::idirectory_iterator<sys::direntry>(),
			std::inserter(orig, orig.end())
		);
	}
	std::set<sys::direntry> copied;
	{
		IStream idir2(otdir);
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

TEST(Directory, OpenClose) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(files.begin(), files.end());
	sys::idirectory dir;
	test_open_close(dir, tdir);
}

TEST(Directory, Iterator) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(files.begin(), files.end());
	test_file_count(tdir, files);
	test_file_list(tdir, files);
	test_file_copy(tdir);
}

void
test_current_dir(const test::tmpdir& tdir) {
	sys::idirtree tree(tdir);
	EXPECT_EQ(tree.current_dir(), static_cast<const sys::path&>(tdir));
}

TEST(DirTree, OpenClose) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(files.begin(), files.end());
	sys::idirtree tree;
	test_open_close(tree, tdir);
	test_current_dir(tdir);
}

TEST(DirTree, Iterator) {
	std::vector<std::string> files {"a", "b", "c"};
	std::vector<std::string> files_d {"e", "f", "g"};
	std::vector<std::string> files_h {"i", "j", "k", "l"};
	test::tmpdir tdir(files.begin(), files.end());
	test::tmpdir tdir_d(
		sys::path(current_test_name(), "d"),
		files_d.begin(),
		files_d.end()
	);
	test::tmpdir tdir_h(
		sys::path(current_test_name(), "h"),
		files_h.begin(),
		files_h.end()
	);
	std::vector<std::string> all_files;
	std::copy(files.begin(), files.end(), std::back_inserter(all_files));
	std::copy(files_d.begin(), files_d.end(), std::back_inserter(all_files));
	std::copy(files_h.begin(), files_h.end(), std::back_inserter(all_files));
	all_files.emplace_back("d");
	all_files.emplace_back("h");

	typedef sys::idirtree_iterator<sys::direntry> direntry_iterator;
	test_file_count<sys::idirtree, direntry_iterator>(tdir, all_files);
	test_file_list<sys::idirtree, direntry_iterator>(tdir, all_files);
}

TEST(ArgStream, All) {
	const std::string arg0 = "Hello!!!";
	const std::string arg1 = "world";
	const int arg2 = 123;
	sys::argstream args;
	EXPECT_EQ(args.argc(), 0);
	args << arg0 << '\0' << arg1 << '\0' << arg2 << '\0';
	EXPECT_EQ(args.argc(), 3);
	EXPECT_EQ(args.argv()[0], arg0);
	EXPECT_EQ(args.argv()[1], arg1);
	EXPECT_EQ(args.argv()[2], std::to_string(arg2));
	EXPECT_EQ(args.argv()[args.argc()], (char*)nullptr);
	args.append(arg0, arg1, arg2);
	EXPECT_EQ(args.argc(), 6);
	EXPECT_EQ(args.argv()[3], arg0);
	EXPECT_EQ(args.argv()[4], arg1);
	EXPECT_EQ(args.argv()[5], std::to_string(arg2));
	EXPECT_EQ(args.argv()[args.argc()], (char*)nullptr);
}

TEST(FileStat, Exists) {
	sys::canonical_path cwd(".");
	sys::path non_existent_file(cwd, "asdasdasd");
	sys::file_stat stat(non_existent_file);
	EXPECT_FALSE(stat.exists());
}

TEST(GetFileType, DirEntry) {
	const char* filename = "get_file_type_1";
	std::ofstream(filename) << "hello world";
	sys::path cwd(".");
	sys::idirectory dir(cwd);
	sys::idirectory_iterator<sys::direntry> end;
	auto result = std::find_if(
		sys::idirectory_iterator<sys::direntry>(dir),
		end,
		[&filename] (const sys::direntry& rhs) {
		    return std::strcmp(rhs.name(), filename) == 0;
		}
	              );
	EXPECT_NE(result, end);
	EXPECT_EQ(sys::file_type::regular, sys::get_file_type(cwd, *result));
}

TEST(GetFileType, PathEntry) {
	const char* filename = "get_file_type_2";
	std::ofstream(filename) << "hello world";
	sys::path cwd(".");
	sys::idirtree dir(cwd);
	sys::idirtree_iterator<sys::pathentry> end;
	auto result =
		std::find_if(
			sys::idirtree_iterator<sys::pathentry>(dir),
			end,
			[&filename] (const sys::pathentry& rhs) {
			    return std::strcmp(rhs.name(), filename) == 0;
			}
		);
	EXPECT_NE(result, end);
	EXPECT_EQ(sys::file_type::regular, sys::get_file_type(*result));
}

TEST(DirTree, CopyRecursively) {
	std::vector<std::string> files1 {"a", "b", "c"};
	std::vector<std::string> files2 {"d", "e", "f"};
	test::tmpdir dir1("dirtree-copy-in", files1.begin(), files1.end());
	test::tmpdir dir2("dirtree-copy-in/next", files2.begin(), files2.end());
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
