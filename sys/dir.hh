#ifndef SYS_DIR_HH
#define SYS_DIR_HH

#include <dirent.h>
#include <cstring>
#include <system_error>
#include <queue>
#include <type_traits>

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
		direntry(direntry&&) = default;
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
		is_hidden() const noexcept {
			return name()[0] == '.';
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

		direntry&
		operator=(const direntry& rhs) = default;

	};

	struct pathentry: public direntry {

		pathentry() = default;
		pathentry(const pathentry&) = default;
		pathentry(pathentry&& rhs) = default;
		~pathentry() = default;

		pathentry(const path& dirname, const direntry& ent):
		direntry(ent),
		_dirname(dirname)
		{}

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

		pathentry&
		operator=(const pathentry&) = default;

		pathentry&
		operator=(pathentry&& rhs) = default;

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

	struct file: public path, public file_stat {

		file() = default;
		file(const file&) = default;
		file(file&&) = default;

		file(const path& dirname, const direntry& ent):
		path(path(dirname, ent.name())),
		file_stat(const_path(*this))
		{}

		bool
		is_hidden() const noexcept {
			const std::string& filepath = path::to_string();
			const size_t pos = filepath.find_last_of(path::separator);
			return pos != std::string::npos && filepath[pos+1] == '.';
		}

		friend std::ostream&
		operator<<(std::ostream& out, const file& rhs) {
			return out << static_cast<const file_stat&>(rhs)
				<< ' ' << static_cast<const path&>(rhs);
		}

		file&
		operator=(const file&) = default;

		file&
		operator=(file&& rhs) = default;

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

	template<class FilePred>
	class basic_directory: public basic_dirstream {

	public:
		typedef FilePred filepred_type;

	private:
		path _dirpath;
		dir_type* _dir = nullptr;
		filepred_type _filepred;

	public:

		basic_directory() = default;

		explicit
		basic_directory(const path& path) {
			open(path);
		}

		basic_directory(basic_directory&& rhs):
		basic_dirstream(std::forward<basic_dirstream>(rhs)),
		_dirpath(std::move(rhs._dirpath)),
		_dir(rhs._dir)
		{ rhs._dir = nullptr; }

		~basic_directory() {
			close();
		}

		void
		open(const path& p) {
			close();
			_dirpath = p;
			_dir = bits::check(
				::opendir(p),
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
				_dir = nullptr;
			}
		}

		bool
		is_open() const noexcept {
			return _dir != nullptr;
		}

		const filepred_type&
		file_pred() const noexcept {
			return _filepred;
		}

		basic_directory&
		operator>>(direntry& rhs) {
			read_direntry(rhs);
			return *this;
		}

		basic_directory&
		operator>>(pathentry& rhs) {
			read_direntry(rhs);
			return *this;
		}

		basic_directory&
		operator>>(file& rhs) {
			read_direntry(rhs);
			return *this;
		}

	private:

		template<class Entry>
		void
		read_direntry(Entry& rhs) {
			if (good()) {
				bool success = false;
				while (!success && !eof()) {
					const direntry* result = static_cast<direntry*>(::readdir(_dir));
					if (!result) {
						_state = state(_state | eofbit);
					} else {
						rhs = Entry(_dirpath, *result);
						if (_filepred(rhs)) {
							success = true;
						}
					}
				}
			}
		}

		void
		read_direntry(direntry& rhs) {
			if (good()) {
				bool success = false;
				while (!success && !eof()) {
					const direntry* result = static_cast<direntry*>(::readdir(_dir));
					if (!result) {
						_state = state(_state | eofbit);
					} else {
						if (_filepred(*result)) {
							rhs = *result;
							success = true;
						}
					}
				}
			}
		}

	};

	struct ignore_hidden_files {

		template<class Ent>
		inline bool
		operator()(const Ent& rhs) const noexcept {
			return !rhs.is_hidden();
		}

	};

	struct ignore_hidden_dirs {

		inline bool
		operator()(const path& prefix, const direntry& rhs) const noexcept {
			return !rhs.is_hidden()
				&& get_file_type(prefix, rhs) == file_type::directory;
		}

		inline bool
		operator()(const path&, const pathentry& rhs) const noexcept {
			return !rhs.is_hidden() && get_file_type(rhs) == file_type::directory;
		}

		inline bool
		operator()(const path&, const file& rhs) const noexcept {
			return !rhs.is_hidden() && rhs.is_directory();
		}

	};

	template<class FilePred, class DirPred>
	struct basic_dirtree: public basic_directory<FilePred> {

		typedef basic_directory<FilePred> directory;
		using basic_dirstream::good;
		using basic_dirstream::eof;
		using basic_dirstream::_state;
		using typename basic_dirstream::state;
		using basic_dirstream::eofbit;
		using basic_dirstream::clear;
		typedef DirPred dir_pred;

		basic_dirtree() = default;

		explicit
		basic_dirtree(const path& starting_point):
		directory(starting_point)
		{ _dirs.emplace(starting_point); }

		void
		open(const path& p) {
			while (!_dirs.empty()) {
				_dirs.pop();
			}
			directory::open(p);
			_dirs.emplace(p);
		}

		const path&
		current_dir() const noexcept {
			return _dirs.front();
		}

		basic_dirtree&
		operator>>(direntry& rhs) {
			read_direntry(rhs);
			return *this;
		}

		basic_dirtree&
		operator>>(pathentry& rhs) {
			read_direntry(rhs);
			return *this;
		}

		basic_dirtree&
		operator>>(file& rhs) {
			read_direntry(rhs);
			return *this;
		}

	private:

		template<class Entry>
		void
		read_direntry(Entry& rhs) {
			bool success = false;
			while (!success && !eof()) {
				if (directory::operator>>(rhs)) {
					success = true;
					const path& cur = current_dir();
					if (_dirpred(cur, rhs)) {
						_dirs.emplace(cur, rhs.name());
					}
				} else {
					_dirs.pop();
					if (_dirs.empty()) {
						_state = state(_state | eofbit);
					} else {
						clear();
						directory::open(_dirs.front());
					}
				}
			}
		}

		std::queue<sys::path> _dirs;
		dir_pred _dirpred;

	};

	typedef basic_directory<ignore_hidden_files> directory;
	typedef basic_dirtree<ignore_hidden_files, ignore_hidden_dirs> dirtree;

	typedef basic_istream_iterator<directory, direntry> directory_iterator;
	template<class T>
	using dirtree_iterator = basic_istream_iterator<dirtree, T>;

}

#endif // SYS_DIR_HH
