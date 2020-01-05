#include <unistdx/ipc/identity>

#include <exception>

void
sys::this_process::set_identity(uid_type uid, gid_type gid) {
    bool group_changed = false;
    gid_type old_gid = group();
    try {
        UNISTDX_CHECK(::setgid(gid));
        group_changed = true;
        UNISTDX_CHECK(::setuid(uid));
    } catch (...) {
        if (group_changed) {
            int ret = ::setgid(old_gid);
            if (ret == -1) {
                /// TODO should we just terminate here?
                // LCOV_EXCL_START
                std::throw_with_nested(bad_call(__FILE__, __LINE__, __func__));
                // LCOV_EXCL_STOP
            }
        }
        throw;
    }
}
