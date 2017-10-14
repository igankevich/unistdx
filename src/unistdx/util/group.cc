#include "group"

#include <algorithm>
#include <ostream>

#include <unistdx/bits/entity>
#include <unistdx/config>
#include <unistdx/it/intersperse_iterator>

std::ostream&
sys::operator<<(std::ostream& out, const group& rhs) {
	out << rhs.name() << ':'
		<< rhs.password() << ':'
		<< rhs.id() << ':';
	std::copy(
		rhs.begin(),
		rhs.end(),
		intersperse_iterator<const char*,char>(out, ',')
	);
	return out;
}


bool
sys::find_group(gid_type gid, group& u) {
	#if defined(UNISTDX_HAVE_GETGRGID_R)
	return find_entity_r<gid_type>(gid, u, u._buf);
	#else
	return find_entity_nr<gid_type>(gid, u, u._buf);
	#endif
}

bool
sys::find_group(const char* name, group& u) {
	#if defined(UNISTDX_HAVE_GETGRNAM_R)
	return find_entity_r<gid_type>(name, u, u._buf);
	#else
	return find_entity_nr<gid_type>(name, u, u._buf);
	#endif
}

