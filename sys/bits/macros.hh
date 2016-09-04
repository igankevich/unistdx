#ifndef SYS_BITS_MACROS_HH
#define SYS_BITS_MACROS_HH

#define SYSX_GCC_VERSION_AT_LEAST(major, minor) \
	((__GNUC__ > major) || (__GNUC__ == major && __GNUC_MINOR__ >= minor))

#endif // SYS_BITS_MACROS_HH
