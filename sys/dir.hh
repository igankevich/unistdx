#ifndef SYS_DIR_HH
#define SYS_DIR_HH

#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <queue>
#include <system_error>

#include <sys/bits/check.hh>
#include <sys/bits/basic_istream_iterator.hh>
#include <sys/bits/basic_ostream_iterator.hh>

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
		is_directory() const noexcept {
			return type() == file_type::directory;
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
		path(dirname, ent.name()),
		file_stat(const_path(*this))
		{}

		bool
		is_hidden() const noexcept {
			return name()[0] == '.';
		}

		const char*
		name() const noexcept {
			const std::string& filepath = path::to_string();
			const size_t pos = filepath.find_last_of(path::separator);
			return pos == std::string::npos
				? filepath.data()
				: (filepath.data() + pos + 1);
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

		void
		setstate(state rhs) noexcept {
			_state = state(_state | rhs);
		}

	protected:

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

		basic_directory(const basic_directory&) = delete;

		~basic_directory() {
			close();
		}

		void
		open(const path& p) {
			close();
			_dirpath = p;
			_dir = ::opendir(p);
			if (!_dir) {
				setstate(failbit);
			}
		}

		void
		close() {
			if (_dir) {
				if (-1 == ::closedir(_dir)) {
					setstate(failbit);
				}
				_dir = nullptr;
			}
		}

		bool
		is_open() const noexcept {
			return _dir != nullptr;
		}

		const filepred_type&
		getfilepred() const noexcept {
			return _filepred;
		}

		void
		setfilepred(filepred_type rhs) {
			_filepred = rhs;
		}

		const path&
		getpath() const noexcept {
			return _dirpath;
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
						setstate(eofbit);
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
						setstate(eofbit);
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

	template<class Trans>
	class basic_odirectory {

	public:
		typedef Trans transform;

	private:
		path _dirpath;
		transform _trans;

	public:
		basic_odirectory() = default;

		basic_odirectory(const path& dir):
		_dirpath(dir)
		{}

		basic_odirectory(basic_odirectory&&) = default;
		basic_odirectory(const basic_odirectory&) = delete;

		void
		open(const path& dir) {
			_dirpath = dir;
		}

		explicit
		operator bool() const noexcept {
			return true;
		}

		bool
		operator!() const noexcept {
			return !operator bool();
		}

		basic_odirectory&
		operator<<(const direntry& rhs) {
			copy_file(
				rhs.name(),
				path(_dirpath, _trans(rhs))
			);
			return *this;
		}

		basic_odirectory&
		operator<<(const pathentry& rhs) {
			copy_file(
				rhs.getpath(),
				path(_dirpath, _trans(rhs))
			);
			return *this;
		}

		basic_odirectory&
		operator<<(const file& rhs) {
			copy_file(
				rhs,
				path(_dirpath, _trans(rhs))
			);
			return *this;
		}

		template<class FilePred>
		basic_odirectory<FilePred>&
		operator<<(basic_directory<FilePred>& rhs) {
			pathentry ent;
			while (rhs >> ent) {
				*this << ent;
			}
			return *this;
		}

	private:
		void
		copy_file(const path& src, const path& dest) {
			std::ofstream(dest) << std::ifstream(src).rdbuf();
		}
	};

	struct copy_verbatim {

		template<class Ent>
		inline sys::path
		operator()(const Ent& rhs) const noexcept {
			return sys::path(rhs.name());
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
		using basic_dirstream::setstate;
		typedef DirPred dirpred_type;

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

		const dirpred_type&
		getdirpred() const noexcept {
			return _dirpred;
		}

		void
		setdirpred(dirpred_type rhs) {
			_dirpred = rhs;
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
						setstate(eofbit);
					} else {
						clear();
						directory::open(_dirs.front());
					}
				}
			}
		}

		std::queue<sys::path> _dirs;
		dirpred_type _dirpred;

	};

	typedef basic_directory<ignore_hidden_files> directory;
	typedef basic_odirectory<copy_verbatim> odirectory;
	typedef basic_dirtree<ignore_hidden_files, ignore_hidden_dirs> dirtree;

	template<class T>
	using idirectory_iterator = basic_istream_iterator<directory, T>;
	template<class T>
	using odirectory_iterator = basic_ostream_iterator<odirectory, T>;
	template<class T>
	using dirtree_iterator = basic_istream_iterator<dirtree, T>;

	typedef idirectory_iterator<direntry> directory_iterator;

}

#endif // SYS_DIR_HH
