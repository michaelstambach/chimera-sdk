// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

// Include Application Headers
#include "test_cluster.h"
#include <stdint.h>

// 'include' from libc
int tolower(int);
//#include <ctype.h>
//#include <stdint.h>

int32_t libraryVariant = 1;

int32_t lowerString(char* str) {
    char* c = str;
    int32_t l = 0;
    while (*c != '\0') {
        *c = tolower(*c);
        l++; c++;
    }
    return l;
}

/**
 * @brief Main function of the cluster test.
 * makes a string lowercase using tolower() from libc
 *
 * @return int returns the string length
 */
int32_t clusterWork(void *args) {
    offloadArgs_t* inputArgs = (offloadArgs_t*) args;
    return lowerString(inputArgs->str);
}