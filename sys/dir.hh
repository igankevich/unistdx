#ifndef SYS_DIR_HH
#define SYS_DIR_HH

#include <dirent.h>
#include <cstring>
#include <system_error>
#include <queue>

#include <sys/bits/check.hh>
#include <sys/bits/basic_istream_iterator.hh>

#include "path.hh"
#include "file.hh"

namespace sys {

	typedef struct ::dirent dirent_type;
	typedef DIR dir_type;
	typedef ::ino_t inode_type;

	struct direntry: public dirent_type {

		constexpr static const char* current_dir = ".";
		constexpr static const char* parent_dir = "..";

		direntry() = default;
		direntry(const direntry&) = default;
		~direntry() = default;

		const char*
		name() const noexcept {
			return this->d_name;
		}

		inode_type
		inode() const noexcept {
			return this->d_ino;
		}

		file_type
		type() const noexcept {
			#if defined(__linux__)
			return file_type(DTTOIF(this->d_type));
			#else
			return file_type(0);
			#endif
		}

		bool
		has_type() const noexcept {
			return type() != file_type(0);
		}

		bool
		is_working_dir() const noexcept {
			return !std::strcmp(name(), current_dir);
		}

		bool
		is_parent_dir() const noexcept {
			return !std::strcmp(name(), parent_dir);
		}

		bool
		operator==(const direntry& rhs) const noexcept {
			return std::strcmp(name(), rhs.name()) == 0;
		}

		bool
		operator!=(const direntry& rhs) const noexcept {
			return std::strcmp(name(), rhs.name()) != 0;
		}

		bool
		operator<(const direntry& rhs) const noexcept {
			return std::strcmp(name(), rhs.name()) < 0;
		}

		friend std::ostream&
		operator<<(std::ostream& out, const direntry& rhs) {
			return out << rhs.name();
		}

	};

	struct pathentry: public direntry {

		pathentry() = default;
		pathentry(const pathentry&) = default;
		~pathentry() = default;

		void
		setdir(const path& rhs) {
			_dirname = rhs;
		}

		const path&
		dirname() const noexcept {
			return _dirname;
		}

		path
		getpath() const {
			return path(_dirname, name());
		}

		bool
		operator<(const pathentry& rhs) const noexcept {
			return _dirname < rhs._dirname;
		}

		bool
		operator==(const pathentry& rhs) const noexcept {
			return _dirname == rhs._dirname;
		}

		bool
		operator!=(const pathentry& rhs) const noexcept {
			return !operator==(rhs);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const pathentry& rhs) {
			return out << rhs.dirname() << path::separator << rhs.name();
		}

	public:

		path _dirname;

	};

	sys::file_type
	get_file_type(const path& dirname, const direntry& entry) {
		return entry.has_type()
			? entry.type()
			: sys::file_stat(path(dirname, entry.name())).type();
	}

	sys::file_type
	get_file_type(const pathentry& rhs) {
	   	return rhs.has_type()
			? rhs.type()
			: sys::file_stat(rhs.getpath()).type();
	}

	struct file: public file_stat, public path {

		file() = default;

		explicit
		file(path&& rhs):
		file_stat(rhs),
		path(rhs)
		{}

		void
		setpath(path&& rhs) {
			file_stat::update(const_path(rhs));
			path::operator=(rhs);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const file& rhs) {
			return out << static_cast<const file_stat&>(rhs)
				<< ' ' << static_cast<const path&>(rhs);
		}

	};

	struct basic_dirstream {

		enum state {
			goodbit = 0,
			failbit = 1,
			badbit = 2,
			eofbit = 4
		};

		explicit
		operator bool() const noexcept {
			return !_state;
		}

		bool
		operator!() const noexcept {
			return !operator bool();
		}

		void
		clear() noexcept {
			_errc = std::errc(0);
			_state = goodbit;
		}

		bool
		good() const noexcept {
			return !_state;
		}

		bool
		bad() const noexcept {
			return _state & badbit;
		}

		bool
		fail() const noexcept {
			return _state & failbit;
		}

		bool
		eof() const noexcept {
			return _state & eofbit;
		}

		state
		rdstate() const noexcept {
			return _state;
		}

		std::errc
		error_code() const noexcept {
			return _errc;
		}

	protected:

		std::errc _errc = std::errc(0);
		state _state = goodbit;

	};

	struct directory: public basic_dirstream {

		template<class Path>
		explicit
		directory(Path&& path) {
			open(std::forward<Path>(path));
		}

		directory(directory&& rhs):
		basic_dirstream(std::forward<basic_dirstream>(rhs)),
		_dir(rhs._dir)
		{ rhs._dir = nullptr; }

		~directory() {
			close();
		}

		template<class Path>
		void
		open(Path&& path) {
			close();
			_dir = bits::check(
				::opendir(const_path(path)),
				__FILE__, __LINE__, __func__
			);
			_errc = std::errc(errno);
		}

		void
		close() {
			if (_dir) {
				bits::check(
					::closedir(_dir),
					__FILE__, __LINE__, __func__
				);
			}
		}

		bool
		is_open() const noexcept {
			return _dir != nullptr;
		}

		directory&
		operator>>(direntry& rhs) {
			if (good()) {
				direntry* result = static_cast<direntry*>(::readdir(_dir));
				if (!result) {
					if (std::errc(errno) == std::errc::bad_file_descriptor) {
						_errc = std::errc(errno);
						_state = state(_state | failbit);
					} else {
						_state = state(_state | eofbit);
					}
				} else {
					rhs = *result;
				}
			}
			return *this;
		}

	private:

		dir_type* _dir = nullptr;

	};

	struct dirtree: public directory {

		template<class Path>
		explicit
		dirtree(Path starting_point):
		directory(starting_point)
		{ _dirs.emplace(starting_point); }

		const path&
		current_path() const noexcept {
			return _dirs.front();
		}

		dirtree&
		operator>>(direntry& rhs) {
			read_direntry(rhs);
			if (good()) {
				path p(current_path(), rhs.name());
				if (determine_file_type(rhs, p) == file_type::directory) {
					_dirs.push(p);
				}
			}
			return *this;
		}

		dirtree&
		operator>>(pathentry& rhs) {
			read_direntry(rhs);
			if (good()) {
				rhs.setdir(current_path());
				path p(std::move(rhs.getpath()));
				if (determine_file_type(rhs, p) == file_type::directory) {
					_dirs.push(p);
				}
			}
			return *this;
		}

		dirtree&
		operator>>(file& rhs) {
			direntry ent;
			read_direntry(ent);
			if (good()) {
				rhs.setpath(path(current_path(), ent.name()));
				if (rhs.is_directory()) {
					_dirs.push(rhs);
				}
			}
			return *this;
		}

	private:

		void
		read_direntry(direntry& rhs) {
			bool success = false;
			while (!success && !eof()) {
				if (directory::operator>>(rhs)) {
					if (!rhs.is_working_dir() && !rhs.is_parent_dir()) {
						success = true;
					}
				} else {
					_dirs.pop();
					if (_dirs.empty()) {
						_state = state(_state | eofbit);
					} else {
						clear();
						open(_dirs.front());
					}
				}
			}
		}

		static file_type
		determine_file_type(const direntry& rhs, const path& p) {
			return rhs.has_type() ? rhs.type() : file_stat(p).type();
		}

		std::queue<sys::path> _dirs;

	};

	typedef basic_istream_iterator<directory, direntry> dirent_iterator;

	template<class T>
	using dirtree_iterator = basic_istream_iterator<dirtree, T>;

}

#endif // SYS_DIR_HH
