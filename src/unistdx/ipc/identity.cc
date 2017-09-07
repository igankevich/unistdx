#include "identity"

void
sys::this_process::set_identity(uid_type uid, gid_type gid) {
	bool b = false;
	uid_type old_uid = user();
	gid_type old_gid = group();
	try {
		UNISTDX_CHECK(::setgid(gid));
		b = true;
		UNISTDX_CHECK(::setuid(uid));
	} catch (...) {
		if (b) {
			UNISTDX_CHECK(::setuid(old_uid));
			UNISTDX_CHECK(::setgid(old_gid));
		} else {
			UNISTDX_CHECK(::setgid(old_gid));
		}
		throw;
	}
}

