// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

#ifndef _TEST_CLUSTER_INCLUDE_GUARD_
#define _TEST_CLUSTER_INCLUDE_GUARD_

#include <stdint.h>

typedef struct {
    void*** output_data;
} offloadArgs_t;


// for static linking host only needs to see entry function
int32_t clusterMain(void* args);

#endif //_TEST_CLUSTER_INCLUDE_GUARD_
