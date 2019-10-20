#include <errno.h>
#include <sys/sendfile.h>

ssize_t
sendfile(int, int, off_t*, size_t) noexcept {
	errno = ENOSYS;
	return -1;
}
