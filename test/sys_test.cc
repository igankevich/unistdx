#include <fstream>
#include <set>
#include <stdx/debug.hh>

#include <sys/dir.hh>
#include <sys/file.hh>
#include <sys/path.hh>
#include <sys/argstream.hh>

#include "test.hh"

namespace test {

	template<class It>
	void
	generate_files(sys::path subdir, It first, It last) {
		::mkdir(subdir, 0755);
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

		template<class It>
		tmpdir(const char* dirname, It first, It last):
		tmpdir(sys::path(dirname), first, last)
		{}

		template<class It>
		tmpdir(sys::path dirname, It first, It last):
		_dirname(dirname)
		{ generate_files(_dirname, first, last); }

		~tmpdir() {
			std::stringstream cmd;
			cmd << "rm -rf '";
			for (char ch : _dirname.to_string()) {
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
			cmd << "find " << __func__;
			std::system(cmd.str().data());
		}

		operator const sys::path&() const noexcept {
			return _dirname;
		}

		const sys::path&
		name() const noexcept {
			return _dirname;
		}

	};

}

void
assert_good(const sys::directory& dir, bool is_open) {
	test::equal(dir.is_open(), is_open, "bad sys::directory::open");
	test::equal(dir.good(), true, "bad sys::directory::good");
	test::equal(dir.bad(), false, "bad sys::directory::bad");
	test::equal(dir.eof(), false, "bad sys::directory::eof");
	test::equal(static_cast<bool>(dir), true, "bad sys::directory::operator bool");
}

template<class Stream>
void
test_open_close(Stream& dir, const test::tmpdir& tdir) {
	assert_good(dir, false);
	dir.open(tdir);
	assert_good(dir, true);
	dir.close();
	assert_good(dir, false);
}

template<class Stream = sys::directory, class Iterator = sys::directory_iterator>
void
test_file_count(const test::tmpdir& tdir, const std::vector<std::string>& files) {
	Stream dir(tdir);
	size_t cnt = std::count_if(
		Iterator(dir),
		Iterator(),
		[] (const sys::direntry& ent) {
			return !ent.is_working_dir() && !ent.is_parent_dir();
		}
	);
	std::stringstream msg;
	msg << "bad file count in " << tdir.name();
	test::equal(cnt, files.size(), msg.str());
}

template<class Stream = sys::directory, class Iterator = sys::directory_iterator>
void
test_total_file_count(const test::tmpdir& tdir, const std::vector<std::string>& files) {
	Stream dir(tdir);
	size_t cnt = std::distance(Iterator(dir), Iterator());
	std::stringstream msg;
	msg << "bad total file count in " << tdir.name();
	test::equal(cnt, files.size(), msg.str());
}

template<class Stream = sys::directory, class Iterator = sys::directory_iterator>
void
test_file_list(const test::tmpdir& tdir, const std::vector<std::string>& files) {
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
	std::stringstream msg;
	msg << "bad file list in " << tdir.name();
	test::compare(files_actual, files_orig, msg.str().data());
}

void
test_directory() {
	std::vector<std::string> files{"a", "b", "c"};
	test::tmpdir tdir(__func__, files.begin(), files.end());
	sys::directory dir;
	test_open_close(dir, tdir);
}

void
test_directory_iterator() {
	std::vector<std::string> files{"a", "b", "c"};
	std::vector<std::string> all_files(files);
	all_files.emplace_back(".");
	all_files.emplace_back("..");
	test::tmpdir tdir(__func__, files.begin(), files.end());
	test_file_count(tdir, files);
	test_total_file_count(tdir, all_files);
	test_file_list(tdir, all_files);
}

void
test_current_dir(const test::tmpdir& tdir) {
	sys::dirtree tree(tdir);
	test::equal(
		tree.current_dir(),
		static_cast<const sys::path&>(tdir),
		"bad sys::dirtree::current_dir"
	);
}

void
test_dirtree() {
	std::vector<std::string> files{"a", "b", "c"};
	test::tmpdir tdir(__func__, files.begin(), files.end());
	sys::dirtree tree;
	test_open_close(tree, tdir);
	test_current_dir(tdir);
}

void
test_dirtree_iterator() {
	std::vector<std::string> files{"a", "b", "c"};
	std::vector<std::string> files_d{"e", "f", "g"};
	std::vector<std::string> files_h{"i", "j", "k", "l"};
	test::tmpdir tdir(__func__, files.begin(), files.end());
	test::tmpdir tdir_d(sys::path(__func__, "d"), files_d.begin(), files_d.end());
	test::tmpdir tdir_h(sys::path(__func__, "h"), files_h.begin(), files_h.end());
	std::vector<std::string> all_files;
	std::copy(files.begin(), files.end(), std::back_inserter(all_files));
	std::copy(files_d.begin(), files_d.end(), std::back_inserter(all_files));
	std::copy(files_h.begin(), files_h.end(), std::back_inserter(all_files));
	all_files.emplace_back("d");
	all_files.emplace_back("h");
	typedef sys::dirtree_iterator<sys::direntry> direntry_iterator;
	test_file_count<sys::dirtree, direntry_iterator>(tdir, all_files);
	test_total_file_count<sys::dirtree, direntry_iterator>(tdir, all_files);
	test_file_list<sys::dirtree, direntry_iterator>(tdir, all_files);
}

void
test_argstream() {
	const std::string arg0 = "Hello!!!";
	const std::string arg1 = "world";
	const int arg2 = 123;
	sys::argstream args;
	test::equal(args.argc(), 0, "bad argc");
	args << arg0 << '\0' << arg1 << '\0' << arg2 << '\0';
	test::equal(args.argc(), 3, "bad argc");
	test::equal(args.argv()[0], arg0, "bad arg0");
	test::equal(args.argv()[1], arg1, "bad arg1");
	test::equal(args.argv()[2], std::to_string(arg2), "bad arg2");
	test::equal(args.argv()[args.argc()], (char*)nullptr, "bad last arg");
	args.append(arg0, arg1, arg2);
	test::equal(args.argc(), 6, "bad argc");
	test::equal(args.argv()[3], arg0, "bad arg0");
	test::equal(args.argv()[4], arg1, "bad arg1");
	test::equal(args.argv()[5], std::to_string(arg2), "bad arg2");
	test::equal(args.argv()[args.argc()], (char*)nullptr, "bad last arg");
}

void
test_file_stat() {
	sys::canonical_path cwd(".");
	sys::path non_existent_file(cwd, "asdasdasd");
	sys::file_stat stat(non_existent_file);
	assert(!stat.exists());
}

void
test_canonical_path_1() {
	sys::canonical_path dir1(".");
	sys::canonical_path dir2("..");
	dir1 = std::move(dir1.dirname());
	test::equal(dir1, dir2, "bad operator=");
}

void
test_canonical_path_2() {
	sys::canonical_path dir1("/tmp");
	sys::canonical_path dir2("/");
	test::equal(dir1.dirname(), dir2, "bad dirname");
	test::equal(dir2.dirname(), dir2, "bad dirname");
	test::equal(dir2.basename(), dir2, "bad basename");
}

void
test_canonical_path_3() {
	sys::canonical_path cwd(".");
	test::equal(sys::canonical_path(cwd.dirname(), cwd.basename()), cwd, "bad dirname/basename");
}

void
test_canonical_path_4() {
	sys::canonical_path root("/");
	sys::canonical_path tmp;
	sys::canonical_path nonexistent(root, "non-existent-directory");
}

void
test_path_1() {
	test::equal(sys::path("/"), sys::path("/"), "bad operator==");
	test::equal(sys::path("/"), std::string("/"), "bad operator==");
	test::equal(sys::path("/"), "/", "bad operator==");
}

void
test_get_file_type_1() {
	const char* filename = __func__;
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
	assert(result != end);
	test::equal(
		sys::get_file_type(cwd, *result),
		sys::file_type::regular,
		"bad get_file_type(..., direntry)"
	);
}

void
test_get_file_type_2() {
	const char* filename = __func__;
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
	assert(result != end);
	test::equal(
		sys::get_file_type(*result),
		sys::file_type::regular,
		"bad get_file_type(pathentry)"
	);
}

int main() {
	test_directory();
	test_directory_iterator();
	test_dirtree();
	test_dirtree_iterator();
	test_argstream();
	test_file_stat();
	test_path_1();
	test_canonical_path_1();
	test_canonical_path_2();
	test_canonical_path_3();
	test_canonical_path_4();
	test_get_file_type_1();
	test_get_file_type_2();
	return 0;
}
