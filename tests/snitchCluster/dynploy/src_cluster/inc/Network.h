
#ifndef __DEEPLOY_HEADER__
#define __DEEPLOY_HEADER__
#include "DeeploySnitchMath.h"
#include "snrt.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
void RunNetwork(uint32_t core_id, uint32_t numThreads);
void InitNetwork(uint32_t core_id, uint32_t numThread);

extern int8_t *DeeployNetwork_MEMORYARENA_L1;
static const uint32_t DeeployNetwork_MEMORYARENA_L1_len = 20480;
extern int8_t *DeeployNetwork_MEMORYARENA_L2;
static const uint32_t DeeployNetwork_MEMORYARENA_L2_len = 5120;
extern int8_t *DeeployNetwork_input_0;
static const uint32_t DeeployNetwork_input_0_len = 1024;
extern int32_t *DeeployNetwork_output_0;
static const uint32_t DeeployNetwork_output_0_len = 1024;
static const uint32_t DeeployNetwork_num_inputs = 1;
static const uint32_t DeeployNetwork_num_outputs = 1;
extern void *DeeployNetwork_inputs[1];
extern void *DeeployNetwork_outputs[1];
static const uint32_t DeeployNetwork_inputs_bytes[1] = {1024};
static const uint32_t DeeployNetwork_outputs_bytes[1] = {4096};
#endif
