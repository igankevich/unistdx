#include "copy_file"

#include <fstream>

#include <unistdx/config>
#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
#include <sys/syscall.h>
#include <unistd.h>
#include <unistdx/fs/file_stat>
#include <unistdx/io/fildes>
#elif defined(UNISTDX_HAVE_SENDFILE)
#include <sys/sendfile.h>
#include <unistdx/fs/file_stat>
#include <unistdx/io/fildes>
#endif

#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
namespace {
	loff_t
	copy_file_range(
		int fd_in,
		loff_t* off_in,
		int fd_out,
		loff_t* off_out,
		size_t len,
		unsigned int flags
	) {
		return syscall(
			__NR_copy_file_range,
			fd_in,
			off_in,
			fd_out,
			off_out,
			len,
			flags
		);
	}

}
#endif

void
sys::copy_file(const path& src, const path& dest) {
	#if defined(UNISTDX_HAVE_SENDFILE) || defined(UNISTDX_HAVE_COPY_FILE_RANGE)
	fildes in(
		src,
		open_flag::read_only |
		open_flag::close_on_exec,
		0644
	);
	fildes out(
		dest,
		open_flag::create |
		open_flag::truncate |
		open_flag::write_only |
		open_flag::close_on_exec,
		0644
	);
	fd_type ifd = in.get_fd();
	fd_type ofd = in.get_fd();
	file_stat st(src);
	offset_type size = st.size();
	ssize_t n;
	#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
	while ((n = ::copy_file_range(ifd, nullptr, ofd, nullptr, size, 0)) > 0) {
		size -= n;
	}
	#elif defined(UNISTDX_HAVE_SENDFILE)
	offset_type offset = 0;
	while ((n = ::sendfile(ofd, ifd, &offset, size)) > 0) {
		size -= n;
	}
	#endif
	if (size != 0) {
		if (errno != EINVAL && errno != ENOSYS) {
			UNISTDX_THROW_BAD_CALL();
		} else {
			std::ofstream(dest) << std::ifstream(src).rdbuf();
		}
	}
	#else
	std::ofstream(dest) << std::ifstream(src).rdbuf();
	#endif // if defined(UNISTDX_HAVE_SENDFILE) ||
	// defined(UNISTDX_HAVE_COPY_FILE_RANGE)
}
