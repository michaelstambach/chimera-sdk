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


int32_t clusterEntry(void *args) {
    return clusterWork(args);
}


int32_t getLibraryVariant() {
    return libraryVariant;
}
