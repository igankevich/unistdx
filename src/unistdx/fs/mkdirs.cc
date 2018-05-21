#include "mkdirs"

#include <sys/stat.h>

#include <unistdx/base/check>

void
sys::mkdirs(sys::path dir, file_mode m, path::size_type offset) {
	const sys::path::size_type n = dir.size();
	bool eof = false;
	for (sys::path::size_type i=0; i<n; ++i) {
		const char ch = dir[i];
		if (i == n-1) {
			eof = true;
		}
		if (ch == '/' || eof) {
			if (!eof) {
				dir[i] = '\0';
			}
			int ret = ::mkdir(dir, m);
			if (!eof) {
				dir[i] = ch;
			}
			if (ret == -1 && errno != EEXIST) {
				UNISTDX_THROW_BAD_CALL();
			}
		}
	}
}
