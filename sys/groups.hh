#ifndef SYS_GROUPS_HH
#define SYS_GROUPS_HH

#include <sys/types.h>
#include <grp.h>
#include <iterator>
#include <ostream>
#include <algorithm>
#include <stdx/iterator.hh>

namespace sys {

	typedef ::gid_t gid_type;

	struct group: public ::group {

		typedef stdx::cstring_iterator<char*> iterator;

		const char* name() const {
			return this->gr_name;
		}

		const char* password() const {
			return this->gr_passwd;
		}

		gid_type id() const {
			return this->gr_gid;
		}

		iterator
		begin() const {
			return iterator(this->gr_mem);
		}

		iterator
		end() const {
			return iterator();
		}

		size_t
		size() const {
			return std::distance(begin(), end());
		}

		bool
		operator==(const group& rhs) const noexcept {
			return id() == rhs.id();
		}

		bool
		operator!=(const group& rhs) const noexcept {
			return !operator==(rhs);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const group& rhs) {
			out << rhs.name() << ':'
				<< rhs.password() << ':'
				<< rhs.id() << ':';
			std::copy(
				rhs.begin(),
				rhs.end(),
				stdx::intersperse_iterator<const char*,char>(out, ',')
			);
			return out;
		}

	};

	static_assert(
		sizeof(group) == sizeof(::group),
		"bad sys::group size"
	);

	namespace bits {
		std::mutex __grentmutex;
	}

	struct groupstream {

		groupstream():
		_lock(bits::__grentmutex),
		_end(false)
		{ ::setgrent(); }

		~groupstream() {
			::endgrent();
		}

		explicit
		operator bool() const noexcept {
			return !_end;
		}

		bool
		operator!() const noexcept {
			return !operator bool();
		}

		groupstream&
		operator>>(group& rhs) {
			if (*this) {
				group* g = next_entry();
				if (g == nullptr) {
					_end = true;
				} else {
					rhs = *g;
				}
			}
			return *this;
		}

	private:

		static group*
		next_entry() {
			return static_cast<group*>(::getgrent());
		}

		std::lock_guard<std::mutex> _lock;
		bool _end;

	};

	typedef basic_istream_iterator<groupstream, group> group_iterator;

}

#endif // SYS_GROUPS_HH
