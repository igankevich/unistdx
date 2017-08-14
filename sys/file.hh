#ifndef SYS_FILE_HH
#define SYS_FILE_HH

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/check>

namespace sys {

	typedef struct ::stat stat_type;
	typedef ::off_t offset_type;
	typedef ::mode_t mode_type;
	typedef ::uid_t uid_type;
	typedef ::gid_t gid_type;

	enum struct file_type: mode_type {
		regular = S_IFREG,
		directory = S_IFDIR,
		block_device = S_IFBLK,
		character_device = S_IFCHR,
		pipe = S_IFIFO,
		socket = S_IFSOCK,
		symbolic_link = S_IFLNK
	};

	char
	to_char(file_type rhs) noexcept;

	inline std::ostream&
	operator<<(std::ostream& out, const file_type& rhs) {
		return out << to_char(rhs);
	}

	struct file_mode {

		static const mode_type mode_mask = 07777;
		static const mode_type perm_mask = 0777;

		enum mask_type {
			specialbits = S_ISUID | S_ISGID | S_ISVTX,
			userbits = S_IRWXU,
			groupbits = S_IRWXG,
			otherbits = S_IRWXO,
			user_rwx = S_IRWXU,
			user_rw = S_IRUSR | S_IWUSR,
			user_r = S_IRUSR,
			user_w = S_IWUSR,
			user_x = S_IXUSR,
			group_rwx = S_IRWXG,
			group_rw = S_IRGRP | S_IWGRP,
			group_r = S_IRGRP,
			group_w = S_IWGRP,
			group_x = S_IXGRP,
			other_rwx = S_IRWXO,
			other_rw = S_IROTH | S_IWOTH,
			other_r = S_IROTH,
			other_w = S_IWOTH,
			other_x = S_IXOTH,
			setuid = S_ISUID,
			setgid = S_ISGID,
			sticky = S_ISVTX
		};

		file_mode() = default;
		~file_mode() = default;

		inline
		file_mode(mode_type rhs) noexcept:
		_mode(rhs)
		{}

		inline
		file_mode(const file_mode& rhs) noexcept:
		_mode(rhs._mode)
		{}

		inline
		operator mode_type&() noexcept {
			return this->_mode;
		}

		inline
		operator mode_type() const noexcept {
			return this->_mode;
		}

		inline file_mode&
		operator=(const file_mode&) = default;

		inline file_mode&
		operator=(mode_type rhs) noexcept {
			this->_mode = rhs;
			return *this;
		}

		inline mode_type
		mode() const noexcept {
			return this->_mode;
		}

		inline mode_type
		mode(mode_type mask) const noexcept {
			return this->_mode & mask;
		}

		inline mode_type
		special() const noexcept {
			return this->mode(specialbits);
		}

		inline mode_type
		user() const noexcept {
			return this->mode(userbits);
		}

		inline mode_type
		group() const noexcept {
			return this->mode(groupbits);
		}

		inline mode_type
		other() const noexcept {
			return this->mode(otherbits);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const file_mode& rhs);

	private:
		mode_type _mode = 0;

	};

	std::ostream&
	operator<<(std::ostream& out, const file_mode& rhs);

	struct file_stat: public stat_type {

		static const mode_type type_mask = S_IFMT;

		inline
		file_stat() noexcept:
		stat_type{}
		{}

		inline explicit
		file_stat(const char* filename):
		stat_type{}
		{ this->update(filename); }

		inline file_type
		type() const noexcept {
			return file_type(this->st_mode & type_mask);
		}

		inline bool
		is_regular() const noexcept {
			return this->type() == file_type::regular;
		}

		inline bool
		is_socket() const noexcept {
			return this->type() == file_type::socket;
		}

		inline bool
		is_symbolic_link() const noexcept {
			return this->type() == file_type::symbolic_link;
		}

		inline bool
		is_block_device() const noexcept {
			return this->type() == file_type::block_device;
		}

		inline bool
		is_directory() const noexcept {
			return this->type() == file_type::directory;
		}

		inline bool
		is_character_device() const noexcept {
			return this->type() == file_type::character_device;
		}

		inline bool
		is_pipe() const noexcept {
			return this->type() == file_type::pipe;
		}

		inline offset_type
		size() const noexcept {
			return this->st_size;
		}

		inline file_mode
		mode() const noexcept {
			return file_mode(this->st_mode);
		}

		inline uid_type
		owner() const noexcept {
			return this->st_uid;
		}

		inline gid_type
		group() const noexcept {
			return this->st_gid;
		}

		inline bool
		exists() const noexcept {
			return this->st_ino != 0;
		}

		inline void
		update(const char* filename) {
			UNISTDX_CHECK_IF_NOT(ENOENT, ::stat(filename, this));
		}

		friend std::ostream&
		operator<<(std::ostream& out, const file_stat& rhs);

	};

	std::ostream&
	operator<<(std::ostream& out, const file_stat& rhs);

}

#endif // SYS_FILE_HH
