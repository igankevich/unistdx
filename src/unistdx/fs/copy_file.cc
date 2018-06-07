#include "copy_file"

#include <unistdx/config>
#include <unistdx/fs/file_stat>
#include <unistdx/io/fildes>

#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
#include <unistd.h>
#endif
#if defined(UNISTDX_HAVE_SENDFILE)
#include <sys/sendfile.h>
#endif

namespace {

	inline void
	do_simple_copy(
		sys::fildes& in,
		sys::fildes& out,
		sys::offset_type file_size
	) {
		const size_t n = 4096;
		char buf[n];
		ssize_t nread = 0;
		do {
			ssize_t m = in.read(buf, n);
			nread += m;
			ssize_t nwritten = 0;
			do { // LCOV_EXCL_LINE
				nwritten += out.write(buf, m);
			} while (nwritten != m);
		} while (nread != file_size);
	}

	#if defined(UNISTDX_HAVE_SENDFILE)
	void
	do_sendfile(
		sys::fildes& in,
		sys::fildes& out,
		sys::offset_type size
	) {
		sys::fd_type ifd = in.fd();
		sys::fd_type ofd = out.fd();
		sys::offset_type offset = 0;
		ssize_t n;
		while ((n = ::sendfile(ofd, ifd, &offset, size)) > 0) {
			size -= n;
		}
		if (size != 0) {
			if (errno != EINVAL && errno != ENOSYS) {
				UNISTDX_THROW_BAD_CALL(); // LCOV_EXCL_LINE
			}
			do_simple_copy(in, out, size);
		}
	}
	#endif

	#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
	inline void
	do_copy_file_range(
		sys::fildes& in,
		sys::fildes& out,
		sys::offset_type size
	) {
		sys::fd_type ifd = in.fd();
		sys::fd_type ofd = out.fd();
		ssize_t n;
		while ((n = ::copy_file_range(ifd, nullptr, ofd, nullptr, size, 0)) > 0) {
			size -= n;
		}
		if (size != 0) {
			if (errno != EXDEV && errno != EBADF && errno != ENOSYS) {
				UNISTDX_THROW_BAD_CALL(); // LCOV_EXCL_LINE
			}
			#if defined(UNISTDX_HAVE_SENDFILE)
			do_sendfile(in, out, size);
			#else
			do_simple_copy(in, out, size);
			#endif
		}
	}
	#endif

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
	const offset_type size = st.size();
	#if defined(UNISTDX_HAVE_COPY_FILE_RANGE)
	do_copy_file_range(in, out, size);
	#elif defined(UNISTDX_HAVE_SENDFILE)
	do_sendfile(in, out, size);
	#else
	do_simple_copy(in, out, size);
	#endif
}
