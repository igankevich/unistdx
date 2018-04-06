#include "terminal"

#include <unistdx/base/check>

sys::path
sys::terminal_device_path(sys::fd_type f) {
	std::string name(64, '\0');
	int ret;
	while (true) {
		ret = ::ttyname_r(f, &name[0], name.size());
		if (ret == 0) {
			break;
		}
		if (errno != ERANGE) {
			UNISTDX_THROW_BAD_CALL();
		}
		name.resize(2*name.size());
	}
	return static_cast<path>(name);
}

