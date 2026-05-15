/*
 * SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Network.h"
#include "dma.h"
#include "snrt.h"
#include "test_cluster.h"
#include "testinputs.h"
#include <stdint.h>

// #define NOPRINT
// #define NOTEST
// #define CI

int clusterMain(void* args) {

  snrt_init();

  uint32_t core_id = snrt_global_core_idx();
  uint32_t compute_core_id = snrt_global_compute_core_idx();

  uint32_t const num_compute_cores = snrt_cluster_compute_core_num();

  if (snrt_is_dm_core()) {
    InitNetwork(core_id, 1);

    // WIESEP: Copy inputs to allocated memory
    for (uint32_t buf = 0; buf < DeeployNetwork_num_inputs; buf++) {
      snrt_dma_start_1d(DeeployNetwork_inputs[buf], testInputVector[buf],
                        DeeployNetwork_inputs_bytes[buf]);
    }
    snrt_dma_wait_all();
  }

  snrt_cluster_hw_barrier();

  snrt_cluster_hw_barrier();
  RunNetwork(compute_core_id, num_compute_cores);

  snrt_cluster_hw_barrier();

  if (snrt_is_dm_core()) {
    offloadArgs_t* offloadArgs = (offloadArgs_t *)args;
    *(offloadArgs->output_data) = DeeployNetwork_outputs;
  }

  snrt_cluster_hw_barrier();
  return 0;
}
