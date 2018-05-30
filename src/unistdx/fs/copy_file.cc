#include "copy_file"

#include <unistdx/config>
#include <unistdx/fs/file_stat>
#include <unistdx/io/fildes>

#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
#include <sys/syscall.h>
#include <unistd.h>
#elif defined(UNISTDX_HAVE_SENDFILE)
#include <sys/sendfile.h>
#endif

namespace {

	#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
	inline loff_t
	copy_file_range_priv(
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
	#endif

	inline void
	copy_file_simple(
		sys::fildes& in,
		sys::fildes& out,
		sys::offset_type file_size
	) {
		const size_t n = 4096;
		char buf[n];
		ssize_t nread = 0;
		do {
			ssize_t m = in.read(buf, n);
			//std::clog << "read " << m << std::endl;
			nread += m;
			ssize_t nwritten = 0;
			do {
				nwritten += out.write(buf, m);
			} while (nwritten != m);
		} while (nread != file_size);
	}

}

void
sys::copy_file(const path& src, const path& dest) {
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
	file_stat st(src);
	offset_type size = st.size();
	#if defined(UNISTDX_HAVE_SENDFILE) || defined(UNISTDX_HAVE_COPY_FILE_RANGE)
	fd_type ifd = in.fd();
	fd_type ofd = out.fd();
	ssize_t n;
	#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
	while ((n = ::copy_file_range_priv(ifd, nullptr, ofd, nullptr, size, 0)) > 0) {
		size -= n;
	}
	if (size != 0) {
		if (errno != EXDEV && errno != EBADF && errno != ENOSYS) {
			UNISTDX_THROW_BAD_CALL();
		}
		copy_file_simple(in, out, size);
	}
	#elif defined(UNISTDX_HAVE_SENDFILE)
	offset_type offset = 0;
	while ((n = ::sendfile(ofd, ifd, &offset, size)) > 0) {
		size -= n;
	}
	if (size != 0) {
		if (errno != EINVAL && errno != ENOSYS) {
			UNISTDX_THROW_BAD_CALL();
		}
		copy_file_simple(in, out, size);
	}
	#endif
	#else
	copy_file_simple(in, out, size);
	#endif // if defined(UNISTDX_HAVE_SENDFILE) ||
	// defined(UNISTDX_HAVE_COPY_FILE_RANGE)
}
