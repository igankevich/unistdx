#include <fstream>
#include <stdx/debug.hh>

#include <sys/dir.hh>
#include <sys/file.hh>
#include <sys/path.hh>
#include <sys/argstream.hh>

#include "test.hh"

void
test_dirent_iterator() {
	sys::const_path path = ".";
//	sys::directory dir(path);
	/*std::copy(
		dir.begin(), dir.end(),
		std::ostream_iterator<sys::direntry>(std::cout, "\n")
	);*/
	/*std::for_each(
		dir.begin(), dir.end(),
		[&path] (const sys::direntry& entry) {
			sys::path p(path, entry.name());
			sys::file_stat f(p);
			std::cout << f << ' ' << p << '\n';
		}
	);*/
	sys::dirtree tree(path);
	std::copy(
		sys::dirtree_iterator<sys::file>(tree),
		sys::dirtree_iterator<sys::file>(),
		std::ostream_iterator<sys::file>(std::cout, "\n")
	);
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
	sys::dirent_iterator end;
	auto result = std::find_if(
		sys::dirent_iterator(dir),
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
	test_dirent_iterator();
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
