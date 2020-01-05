#include <unistdx/bits/safe_calls>

#if defined(UNISTDX_FORK_MUTEX)
sys::bits::global_mutex_type sys::bits::fork_mutex;
#endif
