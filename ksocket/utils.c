#include "utils.h"

uint64_t htonll(uint64_t hostlonglong) { return __builtin_bswap64(hostlonglong); }

#ifndef BUILD_USERMODE
uint32_t htonl(uint32_t hostlong) { return __builtin_bswap32(hostlong); }

uint16_t htons(uint16_t hostshort) { return __builtin_bswap16(hostshort); }
#endif

uint64_t ntohll(uint64_t netlonglong) { return __builtin_bswap64(netlonglong); }

#ifndef BUILD_USERMODE
uint32_t ntohl(uint32_t netlong) { return __builtin_bswap32(netlong); }

uint16_t ntohs(uint16_t netshort) { return __builtin_bswap16(netshort); }
#endif
