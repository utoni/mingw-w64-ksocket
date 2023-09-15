#ifndef KSOCKET_UTILS_H
#define KSOCKET_UTILS_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t htonll(uint64_t hostlonglong);

#ifndef BUILD_USERMODE
uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);
#endif

uint64_t ntohll(uint64_t netlonglong);

#ifndef BUILD_USERMODE
uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);
#endif

#ifdef __cplusplus
}
#endif

#endif
