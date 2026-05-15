/*
 * SPDX-FileCopyrightText: 2022 ETH Zurich and University of Bologna
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DEEPLOY_MATH_HEADER_
#define __DEEPLOY_MATH_HEADER_

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BEGIN_SINGLE_CORE if (core_id == 0) {
#define END_SINGLE_CORE }
#define SINGLE_CORE if (core_id == 0)

#include "macros.h"

// #include "DeeployBasicMath.h"
#include "types.h"

#include "snrt.h"

#include "kernel/Gemm.h"
#include "kernel/Gemm_fp32.h"
#include "kernel/MatMul.h"
#include "kernel/RQGemm.h"
#include "kernel/RQMatMul.h"
#include "kernel/Softmax.h"
#include "kernel/UniformRequantShift.h"
#include "kernel/iNoNorm.h"

#include "dmaStruct.h"

#endif //__DEEPLOY_MATH_HEADER_
