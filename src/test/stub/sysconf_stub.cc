#include <unistd.h>

long int
sysconf(int name) {
	switch (name) {
		case _SC_GETPW_R_SIZE_MAX: return 1;
		case _SC_GETGR_R_SIZE_MAX: return 1;
		case _SC_LOGIN_NAME_MAX: return 128;
		case _SC_NGROUPS_MAX: return 128;
		case _SC_PAGE_SIZE: return 4096;
		default: return -1;
	}
}
