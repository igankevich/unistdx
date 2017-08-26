#include "user"

#include <ostream>
#include <unistdx/bits/entity>
#include <unistdx/config>

std::ostream&
sys::operator<<(std::ostream& out, const user& rhs) {
	return out
		   << rhs.name() << ':'
		   << rhs.password() << ':'
		   << rhs.id() << ':'
		   << rhs.group_id() << ':'
		   << (rhs.real_name() ? rhs.real_name() : "") << ':'
		   << rhs.home() << ':'
		   << rhs.shell();
}

bool
sys::find_user(uid_type uid, user& u) {
	#if defined(UNISTDX_HAVE_GETPWUID_R)
	return find_entity_r<uid_type>(uid, u, u._buf);
	#else
	return find_entity_nr<uid_type>(uid, u, u._buf);
	#endif
}

bool
sys::find_user(const char* name, user& u) {
	#if defined(UNISTDX_HAVE_GETPWNAM_R)
	return find_entity_r<uid_type>(name, u, u._buf);
	#else
	return find_entity_nr<uid_type>(name, u, u._buf);
	#endif
}

