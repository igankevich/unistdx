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

		file_mode(mode_type rhs) noexcept:
		_mode(rhs)
		{}

		file_mode(const file_mode& rhs) noexcept:
		_mode(rhs._mode)
		{}

		operator mode_type&() noexcept {
			return _mode;
		}

		operator mode_type() const noexcept {
			return _mode;
		}

		file_mode&
		operator=(const file_mode&) = default;

		file_mode&
		operator=(mode_type rhs) {
			_mode = rhs;
			return *this;
		}

		mode_type
		mode() const noexcept {
			return _mode;
		}

		mode_type
		mode(mode_type mask) const noexcept {
			return _mode & mask;
		}

		mode_type
		special() const noexcept {
			return mode(specialbits);
		}

		mode_type
		user() const noexcept {
			return mode(userbits);
		}

		mode_type
		group() const noexcept {
			return mode(groupbits);
		}

		mode_type
		other() const noexcept {
			return mode(otherbits);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const file_mode& rhs) {
			rhs.print_special(out);
			rhs.print_user(out);
			rhs.print_group(out);
			rhs.print_other(out);
			return out;
		}

	private:

		void
		print_special(std::ostream& out) const {
			const mode_type s = special();
			out << ((s & setuid) ? 'u' : '-');
			out << ((s & setgid) ? 'g' : '-');
			out << ((s & sticky) ? 't' : '-');
		}

		void
		print_user(std::ostream& out) const {
			const mode_type usr = user();
			out << ((usr & user_r) ? 'r' : '-');
			out << ((usr & user_w) ? 'w' : '-');
			out << ((usr & user_x) ? 'x' : '-');
		}

		void
		print_group(std::ostream& out) const {
			const mode_type grp = group();
			out << ((grp & group_r) ? 'r' : '-');
			out << ((grp & group_w) ? 'w' : '-');
			out << ((grp & group_x) ? 'x' : '-');
		}

		void
		print_other(std::ostream& out) const {
			const mode_type oth = other();
			out << ((oth & other_r) ? 'r' : '-');
			out << ((oth & other_w) ? 'w' : '-');
			out << ((oth & other_x) ? 'x' : '-');
		}

		mode_type _mode = 0;

	};

	struct file_stat: public stat_type {

		static const mode_type type_mask = S_IFMT;

		file_stat():
		stat_type{}
		{}

		explicit
		file_stat(const char* filename):
		stat_type{}
		{ update(filename); }

		file_type
		type() const noexcept {
			return file_type(this->st_mode & type_mask);
		}

		bool
		is_regular() const noexcept {
			return type() == file_type::regular;
		}

		bool
		is_socket() const noexcept {
			return type() == file_type::socket;
		}

		bool
		is_symbolic_link() const noexcept {
			return type() == file_type::symbolic_link;
		}

		bool
		is_block_device() const noexcept {
			return type() == file_type::block_device;
		}

		bool
		is_directory() const noexcept {
			return type() == file_type::directory;
		}

		bool
		is_character_device() const noexcept {
			return type() == file_type::character_device;
		}

		bool
		is_pipe() const noexcept {
			return type() == file_type::pipe;
		}

		offset_type
		size() const noexcept {
			return this->st_size;
		}

		file_mode
		mode() const noexcept {
			return file_mode(this->st_mode);
		}

		uid_type
		owner() const noexcept {
			return this->st_uid;
		}

		gid_type
		group() const noexcept {
			return this->st_gid;
		}

		bool
		exists() const noexcept {
			return this->st_ino != 0;
		}

		void
		update(const char* filename) {
			bits::check_if_not<std::errc::no_such_file_or_directory>(
				::stat(filename, this),
				__FILE__, __LINE__, __func__
			);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const file_stat& rhs) {
			return out << rhs.type() << rhs.mode() << ' '
				<< rhs.owner() << ':' << rhs.group() << ' '
				<< rhs.size();
		}

	};

}

#endif // SYS_FILE_HH
