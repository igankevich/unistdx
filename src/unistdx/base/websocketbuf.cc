#include "websocketbuf"

#include <algorithm>
#include <functional>
#include <ostream>
#include <random>
#include <sstream>

#include <unistdx/base/adapt_engine>
#include <unistdx/base/base64>
#include <unistdx/base/log_message>
#include <unistdx/base/sha1>
#include <unistdx/config>
#include <unistdx/net/bytes>

#ifndef NDEBUG
#include <unistdx/base/log_message>
#endif

namespace {

	typedef sys::adapt_engine<std::random_device,char> engine_type;

	std::random_device rng;

	const char websocket_request[] =
		"GET / HTTP/1.1\r\n"
		"User-Agent: unistdx/" UNISTDX_VERSION "\r\n"
		"Connection: Upgrade\r\n"
		"Upgrade: websocket\r\n"
		"Sec-WebSocket-Protocol: binary\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"Sec-WebSocket-Key: ";

	const char websocket_response[] =
		"HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Protocol: binary\r\n"
		"Sec-WebSocket-Accept: ";

	const char websocket_bad_response[] =
		"HTTP/1.1 400 Bad Request\r\n\r\n";

	const char http_header_separator[] = "\r\n";

	const std::string websocket_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	template <class T>
	inline const T*
	ccast(T* c) {
		return static_cast<const T*>(c);
	}

	template <class T>
	inline std::string
	lowercase_string(const T* first, const T* last) {
		std::string s;
		s.reserve(last-first);
		while (first != last) {
			s.push_back(std::tolower(*first));
			++first;
		}
		return s;
	}

	void
	websocket_accept_header(
		const std::string& web_socket_key,
		char* result
	) {
		using namespace sys;
		bytes<uint32_t[5]> hash;
		sha1 sha;
		sha.put(web_socket_key.data(), web_socket_key.size());
		sha.put(websocket_guid.data(), websocket_guid.size());
		sha.compute();
		sha.digest(hash.value());
		#if !defined(UNISTDX_BIG_ENDIAN)
		for (uint32_t& i : hash.value()) {
			i = to_network_format(i);
		}
		#endif
		base64_encode(hash.begin(), hash.end(), result);
	}

}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::initiate_handshake() {
	this->sputn(websocket_request, sizeof(websocket_request)-1);
	this->put_websocket_key();
	this->sputn("\r\n\r\n", 4);
	this->setstate(client_state::writing_handshake);
	#ifndef NDEBUG
	log_message("ws", ">\n_", std::string(this->pbase(), this->pptr()));
	this->sync();
	log_message("ws", ">\n_", std::string(this->gptr(), this->egptr()));
	#endif
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::write_handshake() {
	this->sync();
	if (this->pptr() == this->pbase()) {
		if (this->_role == role_type::server) {
			this->setstate(server_state::end);
		} else {
			this->setstate(client_state::reading_handshake);
			this->setstate(parse_state::parsing_http_status);
		}
	}
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::client_read_handshake() {
	this->sync();
	parse_state old;
	do {
		old = this->_pstate;
		switch (this->_pstate) {
		case parse_state::initial:
			if (this->_role == role_type::server) {
				this->setstate(parse_state::parsing_http_method);
			} else {
				this->setstate(parse_state::parsing_http_status);
			}
			break;
		case parse_state::parsing_http_method:
			break;
		case parse_state::parsing_http_status:
			this->parse_http_status();
			break;
		case parse_state::parsing_http_headers:
			this->parse_http_headers();
			break;
		case parse_state::end:
			this->setstate(client_state::validating_headers);
			break;
		}
	} while (old != this->_pstate);
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::put_websocket_key() {
	engine_type eng(rng);
	char key[16], encoded[24];
	std::generate_n(key, 16, std::ref(eng));
	sys::base64_encode(key, 16, encoded);
	this->sputn(encoded, 24);
	// save websocket key for validation
	this->_headers["sec-websocket-key"].assign(encoded, 24);
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::put_websocket_accept_header() {
	char encoded[24];
	websocket_accept_header(this->_headers["sec-websocket-key"], encoded);
	this->sputn(encoded, 24);
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::parse_http_method() {
	#ifndef NDEBUG
	sys::log_message("ws", "<\n_", std::string(this->gptr(), this->egptr()));
	#endif
	// find the end of the line
	char_type* result = std::search(
		this->gptr(),
		this->egptr(),
		http_header_separator,
		http_header_separator + sizeof(http_header_separator)-1
	                    );
	if (result != this->egptr()) {
		int cmp;
		const std::streamsize n = result - this->gptr();
		try {
			// skip bad lines
			if (n < 4) {
				throw 0;
			}
			// find http method
			cmp = traits_type::compare(this->gptr(), "GET", 3);
			if (cmp != 0) {
				throw 1;
			}
			this->setstate(parse_state::parsing_http_headers);
		} catch (...) {
			this->_valid = false;
		}
		this->gbump(n+sizeof(http_header_separator)-1);
	}
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::parse_http_status() {
	// find the end of the line
	char_type* result = std::search(
		this->gptr(),
		this->egptr(),
		http_header_separator,
		http_header_separator + sizeof(http_header_separator)-1
	                    );
	if (result != this->egptr()) {
		int cmp;
		const std::streamsize n = result - this->gptr();
		try {
			// skip bad lines
			if (n < 4) {
				throw 0;
			}
			// find protocol name
			cmp = traits_type::compare(this->gptr(), "HTTP", 4);
			if (cmp != 0) {
				throw 1;
			}
			// find HTTP status
			const char_type* result2 = traits_type::find(this->gptr(), n, ' ');
			if (result2 == this->egptr()) {
				throw 2;
			}
			if (this->egptr() - result2 < 3) {
				throw 3;
			}
			cmp = traits_type::compare(result2, "101", 3);
			if (cmp != 0) {
				throw 4;
			}
			this->setstate(parse_state::parsing_http_headers);
		} catch (...) {
			this->_valid = false;
		}
		this->gbump(n+sizeof(http_header_separator)-1);
	}
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::parse_http_headers() {
	// find the end of the line
	char_type* result = std::search(
		this->gptr(),
		this->egptr(),
		http_header_separator,
		http_header_separator + sizeof(http_header_separator)-1
	                    );
	if (result != this->egptr()) {
		const std::streamsize n = result - this->gptr();
		if (n > sizeof(http_header_separator)-1) {
			try {
				const char_type* sep = traits_type::find(this->gptr(), n, ':');
				if (sep == this->egptr()) {
					throw 0;
				}
				this->_headers.emplace(
					lowercase_string(ccast(this->gptr()), sep),
					std::string(sep+2, ccast(result))
				);
			} catch (...) {
				this->_valid = false;
			}
			this->gbump(n+sizeof(http_header_separator)-1);
		}
		if (this->_role == role_type::server) {
			this->setstate(server_state::validating_headers);
		} else {
			this->setstate(client_state::validating_headers);
		}
	}
}

template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::validate_http_headers() {
	bool success = false;
	if (this->_valid) {
		if (this->_role == role_type::server) {
			success = this->has_header("sec-websocket-key") &&
			          this->has_header("sec-websocket-version");
		} else {
			char encoded[24];
			websocket_accept_header(
				this->_headers["sec-websocket-key"],
				encoded
			);
			success = this->has_header(
				"sec-websocket-accept",
				std::string(encoded, 24)
			          );
		}
		success = success &&
		          this->has_header("sec-websocket-protocol", "binary") &&
		          this->has_header("upgrade", "websocket") &&
		          this->has_header("connection", "Upgrade");
	}
	if (success) {
		this->sputn(websocket_response, sizeof(websocket_response)-1);
		this->put_websocket_accept_header();
		this->sputn("\r\n\r\n", 4);
	} else {
		this->_valid = false;
		this->sputn(websocket_bad_response, sizeof(websocket_bad_response)-1);
	}
	this->setstate(server_state::writing_handshake);
}

#ifndef NDEBUG
template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::setstate(server_state s) {
	server_state old = this->_sstate;
	this->_sstate = s;
	sys::log_message("ws", "server state change: _ -> _", int(old), int(s));
}
#endif

#ifndef NDEBUG
template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::setstate(client_state s) {
	client_state old = this->_cstate;
	this->_cstate = s;
	sys::log_message("ws", "client state change: _ -> _", int(old), int(s));
}
#endif

#ifndef NDEBUG
template<class Ch, class Tr, class Fd>
void
sys::basic_websocketbuf<Ch,Tr,Fd>::setstate(parse_state s) {
	parse_state old = this->_pstate;
	this->_pstate = s;
	sys::log_message("ws", "parse state change: _ -> _", int(old), int(s));
}
#endif

template class sys::basic_websocketbuf<char,std::char_traits<char> >;
#ifndef NDEBUG
template class sys::basic_websocketbuf<char,std::char_traits<char>,std::stringbuf >;
#endif
