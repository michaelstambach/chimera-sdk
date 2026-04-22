// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

// Include Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Include Application Headers
#include "test_cluster.h"

// Include Target Specific Headers
#include "soc.h"

// Include Driver Headers
#include "trampoline_snitchCluster.h"

// Include Runtime Headers
// #include "snrt.h"

uint32_t global_var1 = 1;

__thread uint32_t thread_var1 = 1;
__thread uint32_t thread_var2 = 1;

uint32_t incrementGlobalVar() {
    global_var1 += 2;
    return global_var1;
}

uint32_t incrementThreadVar() {
    thread_var2 += 2;
    return thread_var2;
}

uint32_t return42() {
    return 42;
}
