

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

#if defined (VARIANT_MIXED) || defined (VARIANT_DYN)
#define CLUSTER_DYNAMIC
#endif

#ifdef CLUSTER_DYNAMIC
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_start[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_end[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_size[];
#endif

#ifdef VARIANT_MIXED
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work1_mixed_start[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work1_mixed_end[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work1_mixed_size[];

extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work2_mixed_start[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work2_mixed_end[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work2_mixed_size[];
#endif // VARIANT_MIXED

#ifdef VARIANT_DYN
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work1_dyn_start[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work1_dyn_end[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work1_dyn_size[];

extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work2_dyn_start[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work2_dyn_end[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_work2_dyn_size[];

extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_libc_dyn_start[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_libc_dyn_end[];
extern unsigned char _binary_test_snitchCluster_dynlibc_cluster_libc_dyn_size[];
#endif // VARIANT_DYN


void clusterSetup(uint8_t cluster, void *stack_cluster_ptr) {
    void* sp_addr = (void *)(_chimera_clusterBase[cluster] + 0x20000 - 1);
    generate_snitchCluster_SPs_uniform(cluster, sp_addr, 0x2000, stack_cluster_ptr);

    setup_snitchCluster_interruptHandler(clusterInterruptHandler);

    // start the cluster so we can write to its memory
    set_snitchCluster_clockGating(cluster, 0);
    for (volatile int i = 0; i < 10; i++);
    set_snitchCluster_reset(cluster, 0);
}

int32_t clusterOffload(void *args, void* workFunction, uint8_t cluster, void **stack_cluster_ptr) {
    set_snitchCluster_reset(cluster, 1);
    set_snitchCluster_clockGating(cluster, 0);
    
    for (volatile int i = 0; i < 10; i++);
    set_snitchCluster_reset(cluster, 0);

    printf_log("Waiting for cluster to finish...\n");

    // just run on one core for now
    offload_snitchCluster_core(workFunction, args, stack_cluster_ptr[0], cluster, 0);
    uint32_t retVal = wait_snitchCluster_return(cluster);

    set_snitchCluster_clockGating(cluster, 1);

    // printf("Returned value: 0x%08x (%d)\n", retVal, retVal);

    return retVal;
}


int main(void) {

    printf_log("Hello!\n");

#ifdef CLUSTER_DYNAMIC
    printf_log("loading main so\n");
    struct dyn_loaded* dyns_main = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_main, _binary_test_snitchCluster_dynlibc_cluster_start);

    printf_log("loading work so\n");
    struct dyn_loaded* dyns_work = memory_island_malloc(sizeof(struct dyn_loaded));
#ifdef VARIANT_MIXED
    load_so(dyns_work, _binary_test_snitchCluster_dynlibc_cluster_work1_mixed_start);
#endif // VARIANT_MIXED
#ifdef VARIANT_DYN
    load_so(dyns_work, _binary_test_snitchCluster_dynlibc_cluster_work1_dyn_start);
#endif // VARIANT_DYN

    printf_log("performing relocations\n");
    attempt_relocations(dyns_work, dyns_work);
    attempt_relocations(dyns_main, dyns_work);

    uint32_t (*getlibvar)(void) = get_symbol_pointer(dyns_main, "getLibraryVariant");
    if (getlibvar == NULL) {
        printf_log("getLibraryVariant() not found!\n");
        return -1;
    }

    uint32_t lib_var = getlibvar();
    printf_log("Library variant %lu loaded\n", lib_var);
#endif // CLUSTER_DYNAMIC

#ifdef VARIANT_DYN
    printf_log("additionally loading and linking libc\n");
    struct dyn_loaded* dyns_libc = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_libc, _binary_test_snitchCluster_dynlibc_cluster_libc_dyn_start);
    attempt_relocations(dyns_libc, dyns_libc);
    attempt_relocations(dyns_work, dyns_libc);
#endif // VARIANT_DYN

    // cluster setup
    void *stack_cluster1_ptr[_chimera_numCores[CLUSTER1]];
    clusterSetup(CLUSTER1, stack_cluster1_ptr);

    uint32_t retVal;

#ifdef CLUSTER_DYNAMIC
    void (*clusterEntry)(void) = get_symbol_pointer(dyns_main, "clusterEntry");
#endif // CLUSTER_DYNAMIC

    char* strbuf = memory_island_malloc(32);
    strcpy(strbuf, "This is UPPERCASE!");
    offloadArgs_t clusterArgs = {.str = strbuf};

    printf_log("string contents before offloading: %s\n", strbuf);
    printf_log("attempting to execute cluster function on the cluster...\n");
    uint32_t retValCluster = clusterOffload(&clusterArgs, clusterEntry, CLUSTER1, stack_cluster1_ptr);
    printf_log("return value on cluster was: %u\n", retValCluster);
    printf_log("string contents after offloading: %s\n", strbuf);


    return 0;
}
