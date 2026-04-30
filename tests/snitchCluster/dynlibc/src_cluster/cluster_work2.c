// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

// Include Application Headers
#include "test_cluster.h"
#include <stdint.h>

// 'include' from libc
int tolower(int);
//#include <ctype.h>
//#include <stdint.h>

int32_t libraryVariant = 2;

int32_t lowerStringReverse(char* str) {
    char* l = str; // left pointer
    char* r = str; // right pointer
    while (*r++ != '\0'); // move pointer beyond string end
    int32_t len = (r - l - 1) / sizeof(char);
    r -= 2; // last char
    do {
        if (l == r) *r = tolower(*l);
        else {
            char tmp = tolower(*l);
            *l = tolower(*r);
            *r = tmp;
        }
    } while (++l <= --r);
    return len;
}

/**
 * @brief Main function of the cluster test.
 * makes a string lowercase using tolower() from libc
 * AND reverses it!!
 *
 * @return int returns the string length
 */
int32_t clusterWork(void *args) {
    offloadArgs_t* inputArgs = (offloadArgs_t*) args;
    return lowerStringReverse(inputArgs->str);
}