#ifndef SYS_PATH_HH
#define SYS_PATH_HH

#include <limits.h>
#include <stdlib.h>

#include <string>
#include <memory>
#include <algorithm>

namespace sys {

	struct path;

	struct const_path {

		constexpr
		const_path(const char* rhs) noexcept:
		_str(rhs)
		{}

		const_path(const std::string& rhs) noexcept:
		_str(rhs.data())
		{}

		const_path(const path&) noexcept;

		operator const char*() const noexcept {
			return _str;
		}

		bool operator==(const const_path&) = delete;
		bool operator!=(const const_path&) = delete;

		friend std::ostream&
		operator<<(std::ostream& out, const const_path& rhs) {
			return out << rhs._str;
		}

	private:

		const char* _str;

	};

	struct path {

		friend struct const_path;
		friend struct std::hash<path>;

		#if defined(_WIN64) || defined(_WIN32)
		static const char separator = '\\';
		#else
		static const char separator = '/';
		#endif

		path() = default;

		explicit
		path(const char* rhs):
		_path(rhs)
		{}

		explicit
		path(const_path rhs):
		_path(rhs)
		{}

		explicit
		path(std::string&& rhs):
		_path(std::forward<std::string>(rhs))
		{}

		path(const path& rhs):
		_path(rhs._path)
		{}

		path(path&& rhs):
		_path(std::forward<std::string>(rhs._path))
		{}

		template<class A, class B>
		path(A&& dir, B&& filename):
		_path(std::forward<A>(dir))
		{
			_path.push_back(separator);
			_path.append(std::forward<B>(filename));
		}

		operator const char* () const noexcept {
			return _path.data();
		}

		path&
		operator=(const path&) = default;

		bool
		operator==(const path& rhs) const noexcept {
			return _path == rhs._path;
		}

		bool
		operator!=(const path& rhs) const noexcept {
			return !operator==(rhs);
		}

		bool
		operator==(const char* rhs) const noexcept {
			return _path == rhs;
		}

		bool
		operator!=(const char* rhs) const noexcept {
			return !operator==(rhs);
		}

		bool
		operator==(const std::string& rhs) const noexcept {
			return _path == rhs;
		}

		bool
		operator!=(const std::string& rhs) const noexcept {
			return !operator==(rhs);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const path& rhs) {
			return out << rhs._path;
		}

	protected:

		std::string _path;

	};

	const_path::const_path(const path& rhs) noexcept:
	_str(rhs._path.data())
	{}

	struct canonical_path: public path {

		friend struct std::hash<canonical_path>;

		canonical_path() = default;

		canonical_path(path&& rhs):
		path(canonicalise(std::forward<path>(rhs)))
		{}

		template<class A, class B>
		canonical_path(A&& dir, B&& filename):
		path(canonicalise(path(std::forward<A>(dir), std::forward<B>(filename))))
		{}

		canonical_path(canonical_path&& rhs):
		path(std::forward<path>(rhs))
		{}

		canonical_path(const canonical_path& rhs):
		path(rhs)
		{}

		template<class T>
		explicit
		canonical_path(T&& rhs):
		path(canonicalise(path(std::forward<T>(rhs))))
		{}

		canonical_path&
		operator=(canonical_path&&) = default;

		canonical_path&
		operator=(const canonical_path&) = default;

		static path
		canonicalise(path&& rhs) {
			std::unique_ptr<char[]> ptr(new char[PATH_MAX]);
			bits::check<char*>(
				::realpath(const_path(rhs), ptr.get()),
				static_cast<char*>(nullptr),
				__FILE__, __LINE__, __func__
			);
			return path(ptr.get());
		}

		path
		basename() const {
			const size_t pos = _path.find_last_of(path::separator);
			return
				(pos == std::string::npos)
				? path(*this)
				: (pos == 0)
				? path("/")
				: path(std::move(_path.substr(pos+1)));
		}

		canonical_path
		dirname() const {
			const size_t pos = _path.find_last_of(path::separator);
			return
				(pos == std::string::npos)
				? *this
				: (pos == 0)
				? canonical_path("/", 0)
				: canonical_path(std::move(_path.substr(0, pos)), 0);
		}

	private:

		explicit
		canonical_path(const char* str, int):
		path(str)
		{}

		explicit
		canonical_path(std::string&& str, int):
		path(std::forward<std::string>(str))
		{}

	};

}

namespace std {

	template<>
	struct hash<sys::canonical_path> {

		typedef size_t result_type;
		typedef sys::canonical_path argument_type;

		size_t
		operator()(const sys::canonical_path& rhs) const noexcept {
			return std::hash<std::string>()(rhs._path);
		}

	};

	template<>
	struct hash<sys::path> {

		typedef size_t result_type;
		typedef sys::path argument_type;

		size_t
		operator()(const sys::path& rhs) const noexcept {
			return std::hash<std::string>()(rhs._path);
		}

	};

}

#endif // SYS_PATH_HH
