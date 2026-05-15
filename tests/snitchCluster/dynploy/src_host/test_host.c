// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

// Include Standard Libraries

// Include Application Headers
#include "addr_maps/soc_addr_map.h"
#include "test_cluster.h"
#include "cluster_static.h"

// Include Target Specific Headers
#include "soc.h"

// Include Driver Headers
#include "driver.h"

// Include Runtime Headers
#include "log.h"
#include "clint.h"
#include "test_func.h"
#include "util.h"
#include <stdint.h>

// Import HAL Headers

#include "testoutputs.h"

#define CLUSTER1 0
#define STACK_ADDRESS (_chimera_clusterBase[CLUSTER1] + 0x20000 - 1)

extern uintptr_t volatile tohost, fromhost;

// technically these things could change
static const uint32_t num_outputs = 1;
static const uint32_t output_bytes[] = {4096};

#if defined(TARGET_PLATFORM_CHIMERA_CONVOLVE) && defined(HARDWARE_BACKEND_ASIC)
void setGPIO0_UART() {
    // Connect UART port to GPIO 0 Pad
    chimera_padframe_aon_gpio_0_mux_set(CHIMERA_PADFRAME_AON_GPIO_0_group_UART0_port_TX);

    // Set GPIO 0 regs to transmit
    chimera_padframe_aon_gpio_0_cfg_rxe_set(0);  // Disable Pad's Receiver
    chimera_padframe_aon_gpio_0_cfg_trie_set(0); // Disable the tri-state transmitter
}
#endif

int main(void) {
#if defined(TARGET_PLATFORM_CHIMERA_CONVOLVE) && defined(HARDWARE_BACKEND_ASIC)
    // Connect UART to GPIO 0
    setGPIO0_UART();
#endif

    void *stack_cluster_ptr[CLUSTER_0_NUMCORES];
    generate_snitchCluster_SPs_uniform(CLUSTER1, (void *)STACK_ADDRESS, 0x2000, stack_cluster_ptr);

    setup_snitchCluster_interruptHandler(clusterInterruptHandler);

    set_snitchCluster_clockGating(CLUSTER1, 0);

    set_snitchCluster_reset(CLUSTER1, 1);
    for (volatile int i = 0; i < 10; i++);
    set_snitchCluster_reset(CLUSTER1, 0);

    printf_log("Waiting for cluster to finish...\n");

    // Disable interrupts
    set_mie(0);

    void** networkOutput = NULL;

    offloadArgs_t offloadArgs = {.output_data = &networkOutput};
    int32_t (*clusterMain)(void* args) = get_function_pointer(&stack_cluster_ptr[8]);

    offload_snitchCluster(clusterMain, &offloadArgs, stack_cluster_ptr, CLUSTER1);

    // Enable interrupts
    set_mie(1);

    // Handle tohost/fromhost communication
    // the handler is still here to allow for debugging with printf in the cluster code
    // but keep in mind that it only works for the static configuration
    // the dynamic version does not have printf support due to size limitations
    while (snitchCluster_busy(CLUSTER1)) {

        // Wait for tohost to be set by the device
        if (tohost != 0) {
            volatile uint32_t syscall_addr = tohost;

            // Acknowledge tohost
            tohost = 0;

            // printf("Host received tohost: %#x\n", tohost);

            // Cluster does tohost = (uintptr_t)buf->hdr.syscall_mem;
            uint32_t *syscall_mem = (uint32_t *)syscall_addr;

            // printf("Host handling syscall %u: fd=%#x, buf=%p, len=%#x\n", syscall_mem[0],
            //        syscall_mem[1], (void *)syscall_mem[2], syscall_mem[3]);
            if (syscall_mem[0] == 64) { // sys_write
                fwrite((const void *)syscall_mem[2], 1, syscall_mem[3], (FILE *)syscall_mem[1]);
                fflush((FILE *)syscall_mem[1]);
            } else {
                printf_log("Unknown syscall: %u\n", syscall_mem[0]);
            }

            // Notify cluster that syscall is done
            fromhost = syscall_addr;
        }

        // Enter low-power mode until next interrupt or timeout
        clint_sleep_ticks(0, 10);
    }

    uint32_t retVal = wait_snitchCluster_return(CLUSTER1);
    retVal = retVal >> 1;

    set_snitchCluster_clockGating(CLUSTER1, 1);

    printf_log("Returned from cluster: 0x%08x (%d)\n", retVal, retVal);

    printf_log("network output is at %p\n", networkOutput[0]);
    uint8_t* test_val = ((uint8_t *)networkOutput[0]) + 3000;
    printf("test val: %p->%u\n", test_val, *test_val);
    int32_t tot_err = 0;
    uint32_t tot = 0;
    int32_t diff;
    int32_t expected, actual;
    for (uint32_t buf = 0; buf < num_outputs; buf++) {

        tot += output_bytes[buf];
        for (uint32_t i = 0; i < output_bytes[buf]; i++) {
            expected = ((char *)testOutputVector[buf])[i];
            actual = ((char *)networkOutput[buf])[i];
            diff = expected - actual;
            if (diff) {
            tot_err += 1;
            printf_log("Expected: %4d  ", expected);
            printf_log("Actual: %4d  ", actual);
            printf_log("Diff: %4d at Index %12u in Output %u\r\n", diff, i, buf);
            }
        }
    }
    printf_log("Errors: %u out of %u \r\n", tot_err, tot);

    return tot_err;
}