#ifndef SYS_DIR_HH
#define SYS_DIR_HH

#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <queue>
#include <system_error>

#include <sys/bits/basic_istream_iterator.hh>
#include <sys/bits/basic_ostream_iterator.hh>
#include <sys/check>

#include "file.hh"
#include "path"
#include <unistdx_config>

namespace sys {

	typedef struct ::dirent dirent_type;
	typedef DIR dir_type;
	typedef ::ino_t inode_type;

	struct direntry: public dirent_type {

		direntry() = default;
		direntry(const direntry&) = default;
		direntry(direntry&&) = default;
		~direntry() = default;

		inline const char*
		name() const noexcept {
			return this->d_name;
		}

		inline inode_type
		inode() const noexcept {
			return this->d_ino;
		}

		inline file_type
		type() const noexcept {
			#if defined(UNISTDX_HAVE_DTTOIF)
			return file_type(DTTOIF(this->d_type));
			#else
			return file_type(0);
			#endif
		}

		inline bool
		has_type() const noexcept {
			return this->type() != file_type(0);
		}

		inline bool
		is_working_dir() const noexcept {
			return !std::strcmp(this->name(), ".");
		}

		inline bool
		is_parent_dir() const noexcept {
			return !std::strcmp(this->name(), "..");
		}

		inline bool
		is_hidden() const noexcept {
			return this->name()[0] == '.';
		}

		inline bool
		is_directory() const noexcept {
			return this->type() == file_type::directory;
		}

		inline bool
		operator==(const direntry& rhs) const noexcept {
			return std::strcmp(this->name(), rhs.name()) == 0;
		}

		inline bool
		operator!=(const direntry& rhs) const noexcept {
			return std::strcmp(this->name(), rhs.name()) != 0;
		}

		inline bool
		operator<(const direntry& rhs) const noexcept {
			return std::strcmp(this->name(), rhs.name()) < 0;
		}

		inline friend std::ostream&
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

		inline
		pathentry(const path& dirname, const direntry& ent):
		direntry(ent),
		_dirname(dirname) {}

		inline const path&
		dirname() const noexcept {
			return this->_dirname;
		}

		inline path
		getpath() const {
			return path(this->_dirname, name());
		}

		inline bool
		operator<(const pathentry& rhs) const noexcept {
			return this->_dirname < rhs._dirname;
		}

		inline bool
		operator==(const pathentry& rhs) const noexcept {
			return this->_dirname == rhs._dirname;
		}

		inline bool
		operator!=(const pathentry& rhs) const noexcept {
			return !operator==(rhs);
		}

		inline friend std::ostream&
		operator<<(std::ostream& out, const pathentry& rhs) {
			return out << rhs.dirname() << file_separator << rhs.name();
		}

		pathentry&
		operator=(const pathentry&) = default;

		pathentry&
		operator=(pathentry&& rhs) = default;

	public:

		path _dirname;

	};

	inline sys::file_type
	get_file_type(const path& dirname, const direntry& entry) {
		return entry.has_type()
		       ? entry.type()
			   : sys::file_stat(path(dirname, entry.name())).type();
	}

	inline sys::file_type
	get_file_type(const pathentry& rhs) {
		return rhs.has_type()
		       ? rhs.type()
			   : sys::file_stat(rhs.getpath()).type();
	}

	struct file: public path, public file_stat {

		file() = default;
		file(const file&) = default;
		file(file&&) = default;

		inline
		file(const path& dirname, const direntry& ent):
		path(dirname, ent.name()),
		file_stat(*this) {}

		inline bool
		is_hidden() const noexcept {
			return name()[0] == '.';
		}

		inline const char*
		name() const noexcept {
			const size_t pos = this->find_last_of(file_separator);
			return pos == std::string::npos
			       ? this->data()
				   : (this->data() + pos + 1);
		}

		inline friend std::ostream&
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

		inline explicit
		operator bool() const noexcept {
			return !this->_state;
		}

		inline bool
		operator!() const noexcept {
			return !operator bool();
		}

		inline void
		clear() noexcept {
			this->_state = goodbit;
		}

		inline bool
		good() const noexcept {
			return !this->_state;
		}

		inline bool
		bad() const noexcept {
			return this->_state & badbit;
		}

		inline bool
		fail() const noexcept {
			return this->_state & failbit;
		}

		inline bool
		eof() const noexcept {
			return this->_state & eofbit;
		}

		inline state
		rdstate() const noexcept {
			return this->_state;
		}

		inline void
		setstate(state rhs) noexcept {
			this->_state = state(_state | rhs);
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

		inline explicit
		basic_directory(const path& path) {
			this->open(path);
		}

		inline
		basic_directory(basic_directory&& rhs):
		basic_dirstream(std::forward<basic_dirstream>(rhs)),
		_dirpath(std::move(rhs._dirpath)),
		_dir(rhs._dir) {
			rhs._dir = nullptr;
		}

		basic_directory(const basic_directory&) = delete;

		inline
		~basic_directory() {
			this->close();
		}

		inline void
		open(const path& p) {
			this->close();
			this->_dirpath = p;
			this->_dir = ::opendir(p);
			if (!this->_dir) {
				this->setstate(failbit);
			}
		}

		inline void
		close() {
			if (this->_dir) {
				if (-1 == ::closedir(this->_dir)) {
					this->setstate(failbit);
				}
				this->_dir = nullptr;
			}
		}

		inline bool
		is_open() const noexcept {
			return this->_dir != nullptr;
		}

		inline const filepred_type&
		getfilepred() const noexcept {
			return this->_filepred;
		}

		inline void
		setfilepred(filepred_type rhs) {
			this->_filepred = rhs;
		}

		inline const path&
		getpath() const noexcept {
			return this->_dirpath;
		}

		inline basic_directory&
		operator>>(direntry& rhs) {
			this->read_direntry(rhs);
			return *this;
		}

		inline basic_directory&
		operator>>(pathentry& rhs) {
			this->read_direntry(rhs);
			return *this;
		}

		inline basic_directory&
		operator>>(file& rhs) {
			this->read_direntry(rhs);
			return *this;
		}

	private:

		inline direntry*
		read_direntry() noexcept {
			return static_cast<direntry*>(::readdir(this->_dir));
		}

		template<class Entry>
		inline void
		read_direntry(Entry& rhs) {
			if (good()) {
				bool success = false;
				while (!success && !this->eof()) {
					const direntry* result = this->read_direntry();
					if (!result) {
						this->setstate(eofbit);
					} else {
						rhs = Entry(this->_dirpath, *result);
						if (this->_filepred(rhs)) {
							success = true;
						}
					}
				}
			}
		}

		inline void
		read_direntry(direntry& rhs) {
			if (this->good()) {
				bool success = false;
				while (!success && !this->eof()) {
					const direntry* result = this->read_direntry();
					if (!result) {
						this->setstate(eofbit);
					} else {
						if (this->_filepred(*result)) {
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

		inline explicit
		basic_odirectory(const path& dir):
		_dirpath(dir) {}

		basic_odirectory(basic_odirectory&&) = default;
		basic_odirectory(const basic_odirectory&) = delete;

		inline void
		open(const path& dir) {
			this->_dirpath = dir;
		}

		inline explicit
		operator bool() const noexcept {
			return true;
		}

		inline bool
		operator!() const noexcept {
			return !operator bool();
		}

		inline basic_odirectory&
		operator<<(const direntry& rhs) {
			this->copy_file(
				rhs.name(),
				path(this->_dirpath, this->_trans(rhs))
			);
			return *this;
		}

		inline basic_odirectory&
		operator<<(const pathentry& rhs) {
			this->copy_file(
				rhs.getpath(),
				path(this->_dirpath, this->_trans(rhs))
			);
			return *this;
		}

		inline basic_odirectory&
		operator<<(const file& rhs) {
			this->copy_file(
				rhs,
				path(this->_dirpath, this->_trans(rhs))
			);
			return *this;
		}

		template<class FilePred>
		inline basic_odirectory<FilePred>&
		operator<<(basic_directory<FilePred>& rhs) {
			pathentry ent;
			while (rhs >> ent) {
				*this << ent;
			}
			return *this;
		}

	private:
		inline void
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

		inline explicit
		basic_dirtree(const path& starting_point):
		directory(starting_point) {
			this->_dirs.emplace(starting_point);
		}

		inline void
		open(const path& p) {
			while (!this->_dirs.empty()) {
				this->_dirs.pop();
			}
			directory::open(p);
			this->_dirs.emplace(p);
		}

		inline const path&
		current_dir() const noexcept {
			return this->_dirs.front();
		}

		inline const dirpred_type&
		getdirpred() const noexcept {
			return this->_dirpred;
		}

		inline void
		setdirpred(dirpred_type rhs) {
			this->_dirpred = rhs;
		}

		inline basic_dirtree&
		operator>>(direntry& rhs) {
			this->read_direntry(rhs);
			return *this;
		}

		inline basic_dirtree&
		operator>>(pathentry& rhs) {
			this->read_direntry(rhs);
			return *this;
		}

		inline basic_dirtree&
		operator>>(file& rhs) {
			this->read_direntry(rhs);
			return *this;
		}

	private:

		template<class Entry>
		inline void
		read_direntry(Entry& rhs) {
			bool success = false;
			while (!success && !this->eof()) {
				if (this->directory::operator>>(rhs)) {
					success = true;
					const path& cur = this->current_dir();
					if (this->_dirpred(cur, rhs)) {
						this->_dirs.emplace(cur, rhs.name());
					}
				} else {
					this->_dirs.pop();
					if (this->_dirs.empty()) {
						this->setstate(eofbit);
					} else {
						this->clear();
						this->directory::open(this->_dirs.front());
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
