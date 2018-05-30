#include <unistdx/net/pstream>

/**
\example ipacket_and_opacket_guard.cc
\details
Use sys::ipacket_guard and sys::opacket_guard for reading and writing
packets to packet streams.
*/
int main() {
	uint32_t x = 123, y = 0;
	sys::pstream str;
	/// [Write packet]
	try {
		sys::opacket_guard<sys::pstream> g(str);
		str.begin_packet();
		str << x;
		str.end_packet();
	} catch (const std::exception& err) {
		std::cerr << "write error: " << err.what() << std::endl;
	}
	/// [Write packet]
	str.flush();
	/// [Read packet]
	if (str.read_packet()) {
		try {
			sys::ipacket_guard g2(str.rdbuf());
			str >> y;
		} catch (const std::exception& err) {
			std::cerr << "read error: " << err.what() << std::endl;
		}
	}
	/// [Read packet]
	return 0;
}
