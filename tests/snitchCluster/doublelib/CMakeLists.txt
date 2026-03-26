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
# Variant 1: Statically linked with first implementation
add_library(${TEST_NAME}_cluster_static OBJECT src_cluster/test_cluster.c src_cluster/cluster_ret1.c)
target_include_directories(${TEST_NAME}_cluster_static PUBLIC include)

# WIESEP: Set the correct ISA and ABI for the cluster
target_compile_options(${TEST_NAME}_cluster_static
  PRIVATE
  -O2
)
target_link_libraries(${TEST_NAME}_cluster_static PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

## Variant 2: Creating partially linked objects
# # load implementation 2
# add_executable(${TEST_NAME}_cluster_obj2 src_cluster/test_cluster src_cluster/cluster_ret2.c)
# # make output a bare object file
# set_target_properties(${TEST_NAME}_cluster_obj2 PROPERTIES
#   SUFFIX .o
#   ENABLE_EXPORTS TRUE
# )
# # include includes
# target_include_directories(${TEST_NAME}_cluster_obj2 PUBLIC include)
# # link in a relocatable way
# target_link_options(${TEST_NAME}_cluster_obj2 PRIVATE -r)
# # link with cluster runtime (this probably leads to duplication but ok for now)
# target_link_libraries(${TEST_NAME}_cluster_obj2 PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)
# # optimize
# target_compile_options(${TEST_NAME}_cluster_obj2 PRIVATE -O2)
# # embed object file as binary in new object file
# add_custom_command(TARGET ${TEST_NAME}_cluster_obj2 POST_BUILD
#   COMMAND ${CMAKE_OBJCOPY} ARGS -I binary -O elf32-littleriscv ${TEST_NAME}_cluster_obj2.o ${TEST_NAME}_cluster_wrapped2.o
#   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
# )

######## TEST Executable #######################################################
add_chimera_test(${TEST_NAME}_static)

# WIESEP: Link the host and cluster code to the test executable
target_link_libraries(${TEST_NAME}_static PUBLIC ${TEST_NAME}_host)
target_link_libraries(${TEST_NAME}_static PUBLIC ${TEST_NAME}_cluster_static)

# add_chimera_test(${TEST_NAME}_split)
# target_link_libraries(${TEST_NAME}_split PUBLIC ${TEST_NAME}_host)
# target_link_libraries(${TEST_NAME}_split PUBLIC "${CMAKE_BINARY_DIR}/bin/${TEST_NAME}_cluster_wrapped2.o")

