#ifndef TEST_TMPDIR_HH
#define TEST_TMPDIR_HH

#include <fstream>

#include <gtest/gtest.h>

namespace test {

	inline const char*
	current_test_name() {
		return ::testing::UnitTest::GetInstance()->current_test_info()->name();
	}

	template <class It>
	inline void
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

		inline
		tmpdir():
		tmpdir(current_test_name()) {}

		inline explicit
		tmpdir(const char* dirname):
		tmpdir(sys::path(dirname)) {}

		inline explicit
		tmpdir(sys::path dirname):
		_dirname(dirname) {
			mkdir();
		}

		template <class It>
		inline
		tmpdir(It first, It last):
		tmpdir(current_test_name(), first, last) {}

		template <class It>
		inline
		tmpdir(const char* dirname, It first, It last):
		tmpdir(sys::path(dirname), first, last) {}

		template <class It>
		inline
		tmpdir(sys::path dirname, It first, It last):
		_dirname(dirname) {
			mkdir();
			generate_files(_dirname, first, last);
		}

		inline
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

		inline void
		list() {
			std::stringstream cmd;
			cmd << "find " << name();
			(void)std::system(cmd.str().data());
		}

		inline
		operator const sys::path&() const noexcept {
			return _dirname;
		}

		inline const sys::path&
		name() const noexcept {
			return _dirname;
		}

	private:

		inline void
		mkdir() {
			::mkdir(_dirname, 0755);
		}

	};

}

#endif // vim:filetype=cpp
