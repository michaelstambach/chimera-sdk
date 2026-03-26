// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

// Include Application Headers
#include "test_cluster.h"

int32_t libraryVariant = 1;

/**
 * @brief Main function of the cluster test.
 *
 * @return int always returns 1
 */
int32_t clusterWork(void *args) {
    // always return 1
    return 1;
}