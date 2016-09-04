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
test_dirname_basename() {
	sys::canonical_path cwd(".");
	test::equal(sys::canonical_path(cwd.dirname(), cwd.basename()), cwd, "bad dirname/basename");
}

int main() {
	test_dirent_iterator();
	test_argstream();
	test_dirname_basename();
	return 0;
}
