#include "mkdirs"
#include "file_stat"
#include "check"

void
sys::mkdirs(const sys::path& root, const sys::path& relative_path) {
	size_t i0 = 0, i1 = 0;
	while ((i1 = relative_path.find('/', i0)) != std::string::npos) {
		sys::path p(root, relative_path.substr(0, i1));
		file_stat st(p);
		if (!st.exists()) {
			UNISTDX_CHECK(::mkdir(p, 0755));
		}
		i0 = i1 + 1;
	}
}


