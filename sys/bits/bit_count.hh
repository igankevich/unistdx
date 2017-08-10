#ifndef SYS_BITS_BIT_COUNT_HH
#define SYS_BITS_BIT_COUNT_HH

#include <type_traits>
#include "config"

namespace sys {

	namespace bits {

		template<class T>
		inline unsigned int
		bit_count(T value) noexcept {
			static_assert(std::is_unsigned<T>::value, "bad value type");
			unsigned int count = 0;
			while (value > 0) {           // until all bits are zero
				if ((value & 1) == 1)     // check lower bit
					++count;
				value >>= 1;              // shift bits, removing lower bit
			}
			return count;
		}

		#if defined(UNISTDX_HAVE_BUILTIN_POPCOUNT)
		template<>
		inline unsigned int
		bit_count<unsigned int>(unsigned int value) noexcept {
			return __builtin_popcount(value);
		}
		#endif

		#if defined(UNISTDX_HAVE_BUILTIN_POPCOUNTL)
		template<>
		inline unsigned int
		bit_count<unsigned long>(unsigned long value) noexcept {
			return __builtin_popcountl(value);
		}
		#endif

		#if defined(UNISTDX_HAVE_BUILTIN_POPCOUNTLL) && \
		    defined(UNISTDX_HAVE_LONG_LONG)
		template<>
		inline unsigned int
		bit_count<unsigned long long>(unsigned long long value) noexcept {
			return __builtin_popcountll(value);
		}
		#endif

	}

}

#endif // SYS_BITS_BIT_COUNT_HH
