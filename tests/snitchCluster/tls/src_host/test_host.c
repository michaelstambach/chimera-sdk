

// Include Application Headers
#include "addr_maps/soc_addr_map.h"
#include "cluster/offload_snitchCluster.h"
#include "cluster_static.h"

// Include Target Specific Headers
#include "alloc.h"
#include "elf.h"
#include "soc.h"

// Include Driver Headers
#include "driver.h"

// Include Runtime Headers
#include "log.h"
#include "dynamic.h"
#include "test_cluster.h"
#include "trampoline_snitchCluster.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CLUSTER1 0
#define CLUSTER2 1
#define STACK1_ADDRESS (_chimera_clusterBase[CLUSTER1] + 0x20000 - 1)
#define STACK2_ADDRESS (_chimera_clusterBase[CLUSTER2] + 0x20000 - 1)

extern unsigned char _binary_test_snitchCluster_tls_cluster_start[];
extern unsigned char _binary_test_snitchCluster_tls_cluster_end[];
extern unsigned char _binary_test_snitchCluster_tls_cluster_size[];


void clusterSetup(uint8_t cluster, void *stack_cluster_ptr) {
    void* sp_addr = (void *)(_chimera_clusterBase[cluster] + 0x20000 - 1);
    generate_snitchCluster_SPs_uniform(cluster, sp_addr, 0x2000, stack_cluster_ptr);

    setup_snitchCluster_interruptHandler(clusterInterruptHandler);

    // start the cluster so we can write to its memory
    set_snitchCluster_clockGating(cluster, 0);
    for (volatile int i = 0; i < 10; i++);
    set_snitchCluster_reset(cluster, 0);
}

int32_t clusterOffload(void *args, void* workFunction, uint8_t cluster, void *stack_cluster_ptr) {
    set_snitchCluster_reset(cluster, 1);
    set_snitchCluster_clockGating(cluster, 0);
    
    for (volatile int i = 0; i < 10; i++);
    set_snitchCluster_reset(cluster, 0);

    printf_log("Waiting for cluster to finish...\n");

    // just run on one core for now
    offload_snitchCluster_core(workFunction, NULL, stack_cluster_ptr, cluster, 0);
    uint32_t retVal = wait_snitchCluster_return(cluster);

    set_snitchCluster_clockGating(cluster, 1);

    // printf("Returned value: 0x%08x (%d)\n", retVal, retVal);

    return retVal;
}


int main(void) {

    printf_log("Hello!\n");

    printf_log("loading main so\n");
    struct dyn_loaded* dyns_main = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_main, _binary_test_snitchCluster_tls_cluster_start);
    printf_log("performing relocations\n");
    attempt_relocations(dyns_main, dyns_main);
    // void* (*func_tls_get_addr)(tls_index* ti);
    // func_tls_get_addr = __tls_get_addr;
    printf_log("supplying __tls_get_addr()\n");
    relocate_single_symbol(dyns_main, 1, "__tls_get_addr", __tls_get_addr);
    

    void *stack_cluster1_ptr[_chimera_numCores[CLUSTER1]];
    void *stack_cluster2_ptr[_chimera_numCores[CLUSTER2]];
    clusterSetup(CLUSTER1, stack_cluster1_ptr);
    clusterSetup(CLUSTER2, stack_cluster2_ptr);

    printf_log("cluster 1 core 0 stack ends at %p\n", stack_cluster1_ptr[0]);

    // todo: this should happen in some cool function
    *(uint32_t *)stack_cluster1_ptr[0] = 0;
    *(uint32_t *)stack_cluster2_ptr[0] = 0;

    add_tls_module(dyns_main, &stack_cluster1_ptr[0]);
    add_tls_module(dyns_main, &stack_cluster2_ptr[0]);

    uint32_t retVal;

    void (*incrementGlobalVar)(void) = get_symbol_pointer(dyns_main, "incrementGlobalVar");
    void (*incrementThreadVar)(void) = get_symbol_pointer(dyns_main, "incrementThreadVar");

    printf_log("incrementing global var on cluster 1\n");
    retVal = clusterOffload(NULL, incrementGlobalVar, CLUSTER1, stack_cluster1_ptr[0]);
    printf_log("return value on cluster 1 was: %u\n", retVal);

    printf_log("incrementing global var on cluster 2\n");
    retVal = clusterOffload(NULL, incrementGlobalVar, CLUSTER2, stack_cluster2_ptr[0]);
    printf_log("return value on cluster 2 was: %u\n", retVal);

    printf_log("incrementing thread var on cluster 1\n");
    retVal = clusterOffload(NULL, incrementThreadVar, CLUSTER1, stack_cluster1_ptr[0]);
    printf_log("return value on cluster 1 was: %u\n", retVal);

    printf_log("incrementing thread var on cluster 2\n");
    retVal = clusterOffload(NULL, incrementThreadVar, CLUSTER2, stack_cluster2_ptr[0]);
    printf_log("return value on cluster 2 was: %u\n", retVal);

    return 0;
}
