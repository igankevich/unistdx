#ifndef SYS_USERS_HH
#define SYS_USERS_HH

#include <sys/types.h>
#include <pwd.h>
#include <ostream>
#include <sys/bits/basic_istream_iterator.hh>
#include <unistdx_config>

namespace sys {

	typedef ::uid_t uid_type;
	typedef ::gid_t gid_type;

	struct user: public ::passwd {

		const char* name() const {
			return this->pw_name;
		}

		const char* password() const {
			return this->pw_passwd;
		}

		uid_type id() const {
			return this->pw_uid;
		}

		gid_type group_id() const {
			return this->pw_gid;
		}

		const char* real_name() const {
			#if defined(UNISTDX_HAVE_GECOS)
			return this->pw_gecos;
			#else
			return nullptr;
			#endif
		}

		const char* home() const {
			return this->pw_dir;
		}

		const char* shell() const {
			return this->pw_shell;
		}

		bool
		operator==(const user& rhs) const noexcept {
			return id() == rhs.id();
		}

		bool
		operator!=(const user& rhs) const noexcept {
			return !operator==(rhs);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const user& rhs) {
			return out
				<< rhs.name() << ':'
				<< rhs.password() << ':'
				<< rhs.id() << ':'
				<< rhs.group_id() << ':'
				<< (rhs.real_name() ? rhs.real_name() : "") << ':'
				<< rhs.home() << ':'
				<< rhs.shell();
		}

	};

	static_assert(
		sizeof(user) == sizeof(::passwd),
		"bad sys::user size"
	);

	struct userstream {

		userstream():
		_end(false)
		{ ::setpwent(); }

		~userstream() {
			::endpwent();
		}

		explicit
		operator bool() const noexcept {
			return !_end;
		}

		bool
		operator!() const noexcept {
			return !operator bool();
		}

		userstream&
		operator>>(user& rhs) {
			if (*this) {
				user* u = next_entry();
				if (u == nullptr) {
					_end = true;
				} else {
					rhs = *u;
				}
			}
			return *this;
		}

	private:

		static user*
		next_entry() {
			return static_cast<user*>(::getpwent());
		}

		bool _end;

	};

	typedef basic_istream_iterator<userstream, user> user_iterator;

}

#endif // SYS_USERS_HH
