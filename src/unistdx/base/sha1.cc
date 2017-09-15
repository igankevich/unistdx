#include "sha1"

#include <algorithm>
#include <limits>
#include <stdexcept>

#include <unistdx/config>
#include <unistdx/net/byte_order>

#if defined(__GNUC__)
#define ALWAYS_INLINE [[gnu::always_inline]] inline
#else
#define ALWAYS_INLINE inline
#endif

// enable full optimisation
#if defined(__GNUC__)
#pragma GCC optimize("O3", "unroll-all-loops")
#endif

namespace {

	const uint32_t k_0_19 = 0x5a827999;
	const uint32_t k_20_39 = 0x6ed9eba1;
	const uint32_t k_40_59 = 0x8f1bbcdc;
	const uint32_t k_60_79 = 0xca62c1d6;

	// circular left shift
	ALWAYS_INLINE uint32_t
	cls(int n, uint32_t x) noexcept {
		return (x << n) | (x >> (32-n));
	}

	// logical functions {{{
	ALWAYS_INLINE uint32_t
	f_0_19(uint32_t b, uint32_t c, uint32_t d) noexcept {
		return (b & c) | ((~b) & d);
	}

	ALWAYS_INLINE uint32_t
	f_20_39(uint32_t b, uint32_t c, uint32_t d) noexcept {
		return b ^ c ^ d;
	}

	ALWAYS_INLINE uint32_t
	f_40_59(uint32_t b, uint32_t c, uint32_t d) noexcept {
		return (b & c) | (b & d) | (c & d);
	}

	ALWAYS_INLINE uint32_t
	f_60_79(uint32_t b, uint32_t c, uint32_t d) noexcept {
		return f_20_39(b, c, d);
	}

	// }}}

}

#define MAKE_LOOP(from, to) \
	for (int i=from; i<=to; ++i) { \
		temp = cls(5, a) + f_ ## from ## _ ## to(b, c, d) + e + w[i] \
		       + k_ ## from ## _ ## to; \
		e = d; \
		d = c; \
		c = cls(30, b); \
		b = a; \
		a = temp; \
	}

void
sys::sha1::process_block() noexcept {
	// init words
	uint32_t* w = this->_words;
	#if !defined(UNISTDX_BIG_ENDIAN)
	for (int i=0; i<16; ++i) {
		w[i] = to_host_format(w[i]);
	}
	#endif
	for (int i=16; i<=79; ++i) {
		w[i] = cls(1, w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]);
	}
	// process the block
	uint32_t a=this->_digest[0];
	uint32_t b=this->_digest[1];
	uint32_t c=this->_digest[2];
	uint32_t d=this->_digest[3];
	uint32_t e=this->_digest[4];
	uint32_t temp;
	MAKE_LOOP(0, 19);
	MAKE_LOOP(20, 39);
	MAKE_LOOP(40, 59);
	MAKE_LOOP(60, 79);
	// update the digest
	this->_digest[0] += a;
	this->_digest[1] += b;
	this->_digest[2] += c;
	this->_digest[3] += d;
	this->_digest[4] += e;
}

#undef MAKE_LOOP

void
sys::sha1::xput(const char* s, const char* sn, std::size_t n) {
	if (std::numeric_limits<size_t>::max() - n*8 < this->_length) {
		throw std::length_error("sha1 input is too large");
	}
	unsigned char* first = this->_blockptr;
	unsigned char* last = this->block_end();
	while (s != sn) {
		const size_t m = std::min(last - first, sn - s);
		first = std::copy_n(s, m, first);
		s += m;
		if (first == last) {
			this->process_block();
			first = this->_block;
		}
	}
	this->_blockptr = first;
	this->_length += n*8;
}

void
sys::sha1::pad_block() noexcept {
	const uint64_t orig_length = this->_length;
	const int bytes_needed = sizeof(unsigned char) + sizeof(uint64_t);
	const int bytes_avail = this->block_end() - this->_blockptr;
	*this->_blockptr++ = 0x80;
	if (bytes_avail < bytes_needed) {
		// there is not enough space for a 64-bit message size
		std::fill(this->_blockptr, this->block_end(), '\0');
		this->process_block();
		this->_blockptr = this->_block;
	}
	// pad the block
	std::fill(this->_blockptr, this->block_end() - sizeof(uint64_t), '\0');
	// store the size of the original message
	// in the last double word
	this->_dwords[7] = to_network_format(orig_length);
	this->process_block();
}
