#ifndef SYS_BITS_CHECK_HH
#define SYS_BITS_CHECK_HH

#include <stdexcept>
#include <system_error>
#include <ostream>

namespace sys {

	namespace bits {

		struct bad_call: public std::system_error {

			bad_call(const char* file, const int line, const char* function) noexcept:
			std::system_error(errno, std::generic_category()),
			_file(file), _line(line), _function(function)
			{}

			friend std::ostream&
			operator<<(std::ostream& out, const bad_call& rhs) {
				return out
					<< rhs._file << ':'
					<< rhs._line << ':'
					<< rhs._function << ' '
					<< rhs.what();
			}

		private:
			const char* _file;
			const int  _line;
			const char* _function;
		};

		template<class Ret>
		inline Ret
		check(Ret ret, Ret bad, const char* file, const int line, const char* func) {
			if (ret == bad) {
				throw bad_call(file, line, func);
			}
			return ret;
		}

		template<class Ret>
		inline Ret
		check(Ret ret, const char* file, const int line, const char* func) {
			return check<Ret>(ret, Ret(-1), file, line, func);
		}

		template<std::errc ignored_errcode, class Ret>
		inline
		Ret check_if_not(Ret ret, const char* file, const int line, const char* func) {
			return std::errc(errno) == ignored_errcode
				? Ret(0)
				: check<Ret>(ret, file, line, func);
		}

	}

}

#endif // SYS_BITS_CHECK_HH
