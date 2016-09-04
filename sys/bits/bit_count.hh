#ifndef SYS_BITS_BIT_COUNT_HH
#define SYS_BITS_BIT_COUNT_HH

#include <sys/bits/macros.hh>

#if SYSX_GCC_VERSION_AT_LEAST(4, 3)
	#define FACTORY_HAVE_BUILTIN_POPCOUNT
	#define FACTORY_HAVE_BUILTIN_POPCOUNTL
	#if defined(__SIZEOF_LONG_LONG__)
		#define FACTORY_HAVE_BUILTIN_POPCOUNTLL
	#endif
#endif

namespace sys {

	namespace bits {

		template<class T>
		unsigned int
		bit_count(T value) noexcept {
			unsigned int count = 0;
			while (value > 0) {           // until all bits are zero
				if ((value & 1) == 1)     // check lower bit
					++count;
				value >>= 1;              // shift bits, removing lower bit
			}
			return count;
		}

		#if defined(FACTORY_HAVE_BUILTIN_POPCOUNT)
		template<>
		unsigned int
		bit_count<unsigned int>(unsigned int value) noexcept {
			return __builtin_popcount(value);
		}
		#endif

		#if defined(FACTORY_HAVE_BUILTIN_POPCOUNTL)
		template<>
		unsigned int
		bit_count<unsigned long>(unsigned long value) noexcept {
			return __builtin_popcountl(value);
		}
		#endif

		#if defined(FACTORY_HAVE_BUILTIN_POPCOUNTLL)
		template<>
		unsigned int
		bit_count<unsigned long long>(unsigned long long value) noexcept {
			return __builtin_popcountl(value);
		}
		#endif

	}

}

#endif // SYS_BITS_BIT_COUNT_HH
