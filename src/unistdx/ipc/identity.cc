#include <unistdx/ipc/identity>

void
sys::this_process::set_identity(uid_type uid, gid_type gid) {
    // Change uid/gid only if they are different from the current ones.
    // This check is needed when we try to set oveflow_uid/overflow_gid.
    uid_type old_uid = user();
    gid_type old_gid = group();
    if (old_gid != gid) { group(gid); }
    if (old_uid != uid && ::setuid(uid) == -1) {
        if (old_gid != gid) { setgid(old_gid); }
        UNISTDX_THROW_BAD_CALL();
    }
}
