#include <unistdx/fs/mkdirs>

#include <sys/stat.h>
#include <sys/types.h>

#include <unistdx/base/check>
#include <unistdx/fs/file_status>

void
sys::mkdirs(sys::path dir, file_mode m, path::size_type offset) {
    const sys::path::size_type n = dir.size();
    if (n == 0) {
        return;
    }
    for (sys::path::size_type i=offset; i<n-1; ++i) {
        const char ch = dir[i];
        if (ch == '/') {
            dir[i] = '\0';
            int ret = ::mkdir(dir, m);
            dir[i] = ch;
            if (ret == -1 && errno != EEXIST) {
                UNISTDX_THROW_BAD_CALL();
            }
        }
    }
    int ret = ::mkdir(dir, m);
    if (ret == -1) {
        if (errno != EEXIST) {
            UNISTDX_THROW_BAD_CALL();
        }
        file_status st(dir, path_flag::no_follow);
        if (!st.is_directory()) {
            throw bad_call(std::errc::not_a_directory, __FILE__, __LINE__, __func__);
        }
    }
}
