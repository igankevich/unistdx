#include <fstream>
#include <set>
#include <stdx/debug.hh>

#include <gtest/gtest.h>
#include <sys/argstream.hh>
#include <sys/dir.hh>
#include <sys/file.hh>
#include <sys/path>

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
			std::system(cmd.str().data());
		}

		void
		list() {
			std::stringstream cmd;
			cmd << "find " << name();
			std::system(cmd.str().data());
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
assert_good(const sys::directory& dir, bool is_open) {
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

template <class Stream = sys::directory, class Iterator =
			  sys::directory_iterator>
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

template <class Stream = sys::directory, class Iterator =
			  sys::directory_iterator>
void
test_file_list(
	const test::tmpdir& tdir,
	const
	std::vector<std::string>& files
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
	class IStream = sys::directory,
	class OStream = sys::odirectory,
	class Iterator = sys::directory_iterator
>
void
test_file_copy(
	const test::tmpdir& tdir,
	const
	std::vector<std::string>& files
) {
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
	sys::directory dir;
	test_open_close(dir, tdir);
}

TEST(Directory, Iterator) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(files.begin(), files.end());
	test_file_count(tdir, files);
	test_file_list(tdir, files);
	test_file_copy(tdir, files);
}

void
test_current_dir(const test::tmpdir& tdir) {
	sys::dirtree tree(tdir);
	EXPECT_EQ(tree.current_dir(), static_cast<const sys::path&>(tdir));
}

TEST(DirTree, OpenClose) {
	std::vector<std::string> files {"a", "b", "c"};
	test::tmpdir tdir(files.begin(), files.end());
	sys::dirtree tree;
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

	typedef sys::dirtree_iterator<sys::direntry> direntry_iterator;
	test_file_count<sys::dirtree, direntry_iterator>(tdir, all_files);
	test_file_list<sys::dirtree, direntry_iterator>(tdir, all_files);
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
	sys::directory dir(cwd);
	sys::directory_iterator end;
	auto result = std::find_if(
		sys::directory_iterator(dir),
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
	sys::dirtree dir(cwd);
	sys::dirtree_iterator<sys::pathentry> end;
	auto result = std::find_if(
		sys::dirtree_iterator<sys::pathentry>(dir),
		end,
		[&filename] (const sys::pathentry& rhs) {
		    return std::strcmp(rhs.name(), filename) == 0;
		}
	              );
	EXPECT_NE(result, end);
	EXPECT_EQ(sys::file_type::regular, sys::get_file_type(*result));
}
