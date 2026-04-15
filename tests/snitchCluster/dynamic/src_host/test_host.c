

// Include Application Headers
#include "cluster_interrupt.h"

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
#define STACK_ADDRESS (_chimera_clusterBase[CLUSTER1] + 0x20000 - 1)

extern unsigned char _binary_test_snitchCluster_dynamic_cluster_start[];
extern unsigned char _binary_test_snitchCluster_dynamic_cluster_end[];
extern unsigned char _binary_test_snitchCluster_dynamic_cluster_size[];

extern unsigned char _binary_test_snitchCluster_dynamic_cluster_work1_start[];
extern unsigned char _binary_test_snitchCluster_dynamic_cluster_work1_end[];
extern unsigned char _binary_test_snitchCluster_dynamic_cluster_work1_size[];

extern unsigned char _binary_test_snitchCluster_dynamic_cluster_work2_start[];
extern unsigned char _binary_test_snitchCluster_dynamic_cluster_work2_end[];
extern unsigned char _binary_test_snitchCluster_dynamic_cluster_work2_size[];


int32_t clusterOffload(void *args, void* workFunction) {
    void *stack_cluster_ptr[NUM_CLUSTER_CORES];
    generate_snitchCluster_SPs_uniform(CLUSTER1, (void *)STACK_ADDRESS, 0x2000, stack_cluster_ptr);

    setup_snitchCluster_interruptHandler(clusterInterruptHandler);

    set_snitchCluster_clockGating(CLUSTER1, 0);

    set_snitchCluster_reset(CLUSTER1, 1);
    for (volatile int i = 0; i < 10; i++);
    set_snitchCluster_reset(CLUSTER1, 0);

    printf_log("Waiting for cluster to finish...\n");

    offload_snitchCluster(workFunction, NULL, stack_cluster_ptr, CLUSTER1);
    uint32_t retVal = wait_snitchCluster_return(CLUSTER1);

    set_snitchCluster_clockGating(CLUSTER1, 1);

    printf("Returned value: 0x%08x (%d)\n", retVal, retVal);

    return retVal;
}


int main(void) {

    printf_log("Hello!\n");

    printf_log("loading main so\n");
    struct dyn_loaded* dyns_main = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_main, _binary_test_snitchCluster_dynamic_cluster_start);

    printf_log("loading secondary so\n");
    struct dyn_loaded* dyns_prov = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_prov, _binary_test_snitchCluster_dynamic_cluster_work2_start);

    uint32_t (*getlibvar)(void) = get_symbol_pointer(dyns_main, "getLibraryVariant");
    if (getlibvar == NULL) {
        printf_log("getLibraryVariant() not found!\n");
        return -1;
    }

    relocate_global_pointer(dyns_main);
    attempt_relocations(dyns_main, dyns_prov);
    uint32_t lib_var = getlibvar();
    printf_log("after relocation: getLibraryVariant()->%u, libraryVariant points to %p\n", lib_var);

    // attempt offloading the now linked function

    void (*clusterInterruptHandler)(void) = get_symbol_pointer(dyns_main, "clusterInterruptHandler");
    uint32_t (*clusterEntry)(void* args) = get_symbol_pointer(dyns_main, "clusterEntry");

    printf_log("attempting to execute cluster function on the host...\n");
    uint32_t retValHost = clusterEntry(NULL);
    printf_log("return value on host was: %u\n", retValHost);

    printf_log("attempting to execute cluster function on the cluster...\n");
    uint32_t retValCluster = clusterOffload(NULL, clusterEntry);
    printf_log("return value on cluster was: %u\n", retValCluster);


    return 0;
}
