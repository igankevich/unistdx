#include "family"

#include <limits>

#include <unistdx/base/types>

namespace {

	typedef sys::u8 raw_family_type;

	enum struct portable_family_type: raw_family_type {
		unspecified = 0,
		inet = 1,
		inet6 = 2,
		unix = 3
		#if defined(UNISTDX_HAVE_NETLINK)
		, netlink = 4
		#endif
	};

	inline portable_family_type
	map_family_type(sys::family_type t) {
		switch (t) {
			case sys::family_type::unspecified: return portable_family_type::unspecified;
			case sys::family_type::inet: return portable_family_type::inet;
			case sys::family_type::inet6: return portable_family_type::inet6;
			case sys::family_type::unix: return portable_family_type::unix;
			#if defined(UNISTDX_HAVE_NETLINK)
			case sys::family_type::netlink: return portable_family_type::netlink;
			#endif
			default: return portable_family_type(
				std::numeric_limits<sys::sa_family_type>::max()
			);
		}
	};

	inline sys::family_type
	map_family_type(portable_family_type t) {
		switch (t) {
			case portable_family_type::unspecified: return sys::family_type::unspecified;
			case portable_family_type::inet: return sys::family_type::inet;
			case portable_family_type::inet6: return sys::family_type::inet6;
			case portable_family_type::unix: return sys::family_type::unix;
			#if defined(UNISTDX_HAVE_NETLINK)
			case portable_family_type::netlink: return sys::family_type::netlink;
			#endif
			default: return sys::family_type(
				std::numeric_limits<sys::sa_family_type>::max()
			);
		}
	};

}

sys::bstream&
sys::operator<<(bstream& out, family_type rhs) {
	return out << static_cast<raw_family_type>(map_family_type(rhs));
}

sys::bstream&
sys::operator>>(bstream& in, family_type& rhs) {
	raw_family_type raw;
	in >> raw;
	rhs = map_family_type(static_cast<portable_family_type>(raw));
	return in;
}

const char*
sys::to_string(family_type rhs) {
	switch (rhs) {
	case family_type::unspecified: return "unspecified";
	case family_type::inet: return "inet";
	case family_type::inet6: return "inet6";
	case family_type::unix: return "unix";
	#if defined(UNISTDX_HAVE_NETLINK)
	case family_type::netlink: return "netlink";
	#endif
	default: return "unknown";
	}
}

std::ostream&
sys::operator<<(std::ostream& out, const family_type& rhs) {
	return out << to_string(rhs);
}

