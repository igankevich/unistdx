#include "mkdirs"

#include <sys/stat.h>

#include <unistdx/base/check>

void
sys::mkdirs(const sys::path& root, const sys::path& relative_path) {
	size_t i0 = 0, i1 = 0;
	while ((i1 = relative_path.find('/', i0)) != std::string::npos) {
		sys::path p(root, relative_path.substr(0, i1));
		int ret = ::mkdir(p, 0755);
		if (ret == -1 && errno != EEXIST) {
			UNISTDX_THROW_BAD_CALL();
		}
		i0 = i1 + 1;
	}
}


void
sys::mkdirs(sys::path dir) {
	dir += '/';
	size_t i0 = 0, i1 = 0;
	while ((i1 = dir.find('/', i0)) != std::string::npos) {
		sys::path p(dir.substr(0, i1));
		int ret = ::mkdir(p, 0755);
		if (ret == -1 && errno != EEXIST) {
			UNISTDX_THROW_BAD_CALL();
		}
		i0 = i1 + 1;
	}
}
