// SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
// SPDX-License-Identifier: Apache-2.0

#ifndef _TEST_FUNC_INCLUDE_GUARD_
#define _TEST_FUNC_INCLUDE_GUARD_

#include <stdint.h>

// this function provides a pointer to the function which is to be offloaded
// depending on whether we are compiling for the static or dynamic case this
// function will do something else
/// get_function_pointer takes no parameters and returns a pointer to a
/// function which also takes no parameters and returns an int32_t
int32_t (*get_function_pointer(void** stack_ptr))(void* args);

#endif //_TEST_FUNC_INCLUDE_GUARD_
