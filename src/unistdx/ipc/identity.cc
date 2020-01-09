#include <unistdx/ipc/identity>

void
sys::this_process::set_identity(uid_type uid, gid_type gid) {
    gid_type old_gid = group();
    group(gid);
    if (::setuid(uid) == -1) {
        setgid(old_gid);
        UNISTDX_THROW_BAD_CALL();
    }
}
