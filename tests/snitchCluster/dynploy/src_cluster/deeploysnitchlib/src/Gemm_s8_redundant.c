/*
 * SPDX-FileCopyrightText: 2025 ETH Zurich and University of Bologna
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DeeploySnitchMath.h"
#include "kernel/Gemm.h"
#include "team.h"
#include <stdint.h>

void Gemm_s8_row_parallel(int8_t const *__restrict__ pSrcA,
                          int8_t const *__restrict__ pSrcB,
                          int32_t const *__restrict__ pSrcC,
                          int32_t *__restrict__ pDstY, uint32_t M, uint32_t N,
                          uint32_t O, int32_t alpha, int32_t beta) {
  uint32_t core_id = snrt_cluster_core_idx();
  uint32_t numThreads = snrt_cluster_compute_core_num();

  // Parallelize by assigning each core a row tile
  uint32_t const MQuotient = M / numThreads;
  uint32_t const MRemainder = M % numThreads;
  uint32_t const MSize = MQuotient + (core_id < MRemainder ? 1 : 0);
  uint32_t const MStart =
      core_id * MQuotient + (core_id < MRemainder ? core_id : MRemainder);
  uint32_t const MEnd = MStart + MSize;

  for (uint32_t m = MStart; m < MEnd; m++) {
    for (uint32_t o = 0; o < O; o++) {
      // calculate twice
      volatile int32_t sum = 0;

      for (uint32_t n = 0; n < N; ++n) {
        sum += (int32_t)pSrcA[m * N + n] * pSrcB[n * O + o];
      }
      sum = alpha * sum + beta * pSrcC[m * O + o];

      volatile int32_t sum1 = sum;
      sum = 0;

      for (uint32_t n = 0; n < N; ++n) {
        sum += (int32_t)pSrcA[m * N + n] * pSrcB[n * O + o];
      }
      sum = alpha * sum + beta * pSrcC[m * O + o];

      volatile int32_t diff = sum1 - sum;

      if (diff == 0) {
        pDstY[m * O + o] = sum;
      } else {
        // fall back to zero if the results are not equal
        pDstY[m * O + o] = 0;
      }
    }
  }
}

void Gemm_s8_transB_row_parallel(int8_t const *__restrict__ pSrcA,
                                 int8_t const *__restrict__ pSrcB,
                                 int32_t const *__restrict__ pSrcC,
                                 int32_t *__restrict__ pDstY, uint32_t M,
                                 uint32_t N, uint32_t O, int32_t alpha,
                                 int32_t beta) {
  uint32_t core_id = snrt_cluster_core_idx();
  uint32_t numThreads = snrt_cluster_compute_core_num();

  // Parallelize by assigning each core a row tile
  uint32_t const MQuotient = M / numThreads;
  uint32_t const MRemainder = M % numThreads;
  uint32_t const MSize = MQuotient + (core_id < MRemainder ? 1 : 0);
  uint32_t const MStart =
      core_id * MQuotient + (core_id < MRemainder ? core_id : MRemainder);
  uint32_t const MEnd = MStart + MSize;

  for (uint32_t m = MStart; m < MEnd; m++) {
    for (uint32_t o = 0; o < O; o++) {
      // same as above
      volatile int32_t sum = 0;

      for (uint32_t n = 0; n < N; ++n) {
        sum += (int32_t)pSrcA[m * N + n] * pSrcB[o * N + n];
      }
      sum = alpha * sum + beta * pSrcC[m * O + o];

      int32_t sum1 = sum;
      sum = 0;

      for (uint32_t n = 0; n < N; ++n) {
        sum += (int32_t)pSrcA[m * N + n] * pSrcB[o * N + n];
      }
      sum = alpha * sum + beta * pSrcC[m * O + o];

      volatile int32_t diff = sum1 - sum;

      if (diff == 0) {
        pDstY[m * O + o] = sum1;
      } else {
        pDstY[m * O + o] = 0;
      }
    }
  }
}
