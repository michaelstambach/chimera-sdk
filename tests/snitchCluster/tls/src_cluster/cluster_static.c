#include "cluster_static.h"
#include "soc_addr_map.h"
#include "soc_regs.h"
#include "string.h"
#include "test_cluster.h"
#include "trampoline_snitchCluster.h"

/**
 * @brief Interrupt handler for the cluster, which clears the interrupt flag for the current hart.
 *
 * @warning Stack, thread and global pointer might not yet be set up!
 */
__attribute__((naked)) void clusterInterruptHandler() {
    _SET_CLUSTER_BUSY();
    _SETUP_GP();

    asm volatile(
        // Load mhartid CSR into t0
        "csrr t0, mhartid\n"

        // Load clint base address into t1
        "la t1, __base_clint\n"

        // Calculate the interrupt target address: t1 = t1 + (t0 * 4)
        "slli t0, t0, 2\n"
        "add t1, t1, t0\n"
        // Store 0 to the interrupt target address
        "sw zero, 0(t1)\n"
        "ret"
        :            // No outputs
        :            // No inputs
        : "t0", "t1" // Declare clobbered registers
    );
}

void *__tls_get_addr(tls_index *ti) {
    void** tp = __builtin_thread_pointer();
    return (void *)(tp[ti->ti_module] + ti->ti_offset);
}

void cluster_add_dtv_entry(void* args) {
    add_dtv_entry_args_t *argsStruct = (add_dtv_entry_args_t *)args;
    void** tp = __builtin_thread_pointer();
    tp[argsStruct->index] = argsStruct->value;
}
