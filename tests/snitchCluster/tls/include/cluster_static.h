
#ifndef _CLUSTER_STATIC_INCLUDE_GUARD_
#define _CLUSTER_STATIC_INCLUDE_GUARD_


#include <stdint.h>
typedef struct {
    uint32_t ti_module;
    uint32_t ti_offset;
} tls_index;

typedef struct {
    uint32_t index;
    void* value;
} add_dtv_entry_args_t;

void clusterInterruptHandler();
void *__tls_get_addr(tls_index *ti);
void cluster_add_dtv_entry(void* args);

#endif // _CLUSTER_STATIC_INCLUDE_GUARD_

