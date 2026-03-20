# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

set(TEST_NAME test_snitchCluster_doublelib)

######## HOST Code #############################################################
file(GLOB_RECURSE TEST_HOST_SRCS
  "src_host/*.c"
)

add_library(${TEST_NAME}_host OBJECT ${TEST_HOST_SRCS})
target_include_directories(${TEST_NAME}_host PUBLIC include)

# WIESEP: Set the correct ISA and ABI for the host
target_compile_options(${TEST_NAME}_host
  PRIVATE
  -O2
)
target_link_libraries(${TEST_NAME}_host PUBLIC runtime_host picolibc_host trampoline_rv32im)

######## CLUSTER Code ##########################################################
# Variant 1
add_library(${TEST_NAME}_cluster_1 OBJECT src_cluster/test_cluster.c src_cluster/cluster_ret1.c)
target_include_directories(${TEST_NAME}_cluster_1 PUBLIC include)

# WIESEP: Set the correct ISA and ABI for the cluster
target_compile_options(${TEST_NAME}_cluster_1
  PRIVATE
  -O2
)
target_link_libraries(${TEST_NAME}_cluster_1 PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

# Variant 1
add_library(${TEST_NAME}_cluster_2 OBJECT src_cluster/test_cluster.c src_cluster/cluster_ret2.c)
target_include_directories(${TEST_NAME}_cluster_2 PUBLIC include)

# WIESEP: Set the correct ISA and ABI for the cluster
target_compile_options(${TEST_NAME}_cluster_2
  PRIVATE
  -O2
)
target_link_libraries(${TEST_NAME}_cluster_2 PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

######## TEST Executable #######################################################
add_chimera_test(
  ${TEST_NAME}_1
)

# WIESEP: Link the host and cluster code to the test executable
target_link_libraries(${TEST_NAME}_1 PUBLIC ${TEST_NAME}_host)
target_link_libraries(${TEST_NAME}_1 PUBLIC ${TEST_NAME}_cluster_1)

add_chimera_test(
  ${TEST_NAME}_2
)

# WIESEP: Link the host and cluster code to the test executable
target_link_libraries(${TEST_NAME}_2 PUBLIC ${TEST_NAME}_host)
target_link_libraries(${TEST_NAME}_2 PUBLIC ${TEST_NAME}_cluster_2)

