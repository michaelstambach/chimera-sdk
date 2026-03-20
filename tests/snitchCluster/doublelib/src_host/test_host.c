

// Include Application Headers
#include "test_cluster.h"

// Include Target Specific Headers
#include "soc.h"

// Include Driver Headers
#include "driver.h"

// Include Runtime Headers
#include "log.h"

#define CLUSTER1 0
#define STACK_ADDRESS (_chimera_clusterBase[CLUSTER1] + 0x20000 - 1)

int main(void) {
    void *stack_cluster_ptr[NUM_CLUSTER_CORES];
    generate_snitchCluster_SPs_uniform(CLUSTER1, (void *)STACK_ADDRESS, 0x2000, stack_cluster_ptr);

    setup_snitchCluster_interruptHandler(clusterInterruptHandler);

    set_snitchCluster_clockGating(CLUSTER1, 0);

    set_snitchCluster_reset(CLUSTER1, 1);
    for (volatile int i = 0; i < 10; i++);
    set_snitchCluster_reset(CLUSTER1, 0);

    printf_log("Waiting for cluster to finish...\n");

    offload_snitchCluster(testReturn, 0, stack_cluster_ptr, CLUSTER1);
    uint32_t retVal = wait_snitchCluster_return(CLUSTER1);

    set_snitchCluster_clockGating(CLUSTER1, 1);

    printf("Returned value: 0x%08x (%d)\n", retVal, retVal);

    return 0;
}
