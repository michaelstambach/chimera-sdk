# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

set(TEST_NAME test_snitchCluster_dynamic)

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

######## TEST Executable #######################################################
add_chimera_test(
  ${TEST_NAME}
)

# WIESEP: Link the host and cluster code to the test executable
target_link_libraries(${TEST_NAME} PUBLIC ${TEST_NAME}_host)
target_link_libraries(${TEST_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src_cluster/test_cluster_so_bin.o")

