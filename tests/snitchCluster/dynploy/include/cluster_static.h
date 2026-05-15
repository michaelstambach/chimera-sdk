
#ifndef _CLUSTER_STATIC_INCLUDE_GUARD_
#define _CLUSTER_STATIC_INCLUDE_GUARD_


#include <stdint.h>

void clusterInterruptHandler();

// tls
typedef struct {
    uint32_t ti_module;
    uint32_t ti_offset;
} tls_index;
void *__tls_get_addr(tls_index *ti);

#endif // _CLUSTER_STATIC_INCLUDE_GUARD_

