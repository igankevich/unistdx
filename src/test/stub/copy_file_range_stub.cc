#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

ssize_t
copy_file_range(int, loff_t*, int, loff_t*, size_t, unsigned int) {
    errno = ENOSYS;
    return -1;
}
