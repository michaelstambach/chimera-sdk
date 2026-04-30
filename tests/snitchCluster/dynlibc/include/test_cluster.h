// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

#ifndef _TEST_CLUSTER_INCLUDE_GUARD_
#define _TEST_CLUSTER_INCLUDE_GUARD_

#include <stdint.h>


int32_t clusterEntry(void *args);
int32_t getLibraryVariant();

extern int32_t libraryVariant;

int32_t clusterWork(void *args);
// int32_t clusterOffload(void *args);

typedef struct {
    char* str;
} offloadArgs_t;

#endif //_TEST_CLUSTER_INCLUDE_GUARD_
