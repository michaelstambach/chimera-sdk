# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

set(TEST_NAME test_snitchCluster_tls)

# get_property(SHARED_SUPPORT GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
# message("supports shared libs: ${SHARED_SUPPORT}")

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

#### statically link some cluster code

add_library(${TEST_NAME}_cluster_static OBJECT src_cluster/cluster_static.c)
target_include_directories(${TEST_NAME}_cluster_static PUBLIC include)

# WIESEP: Set the correct ISA and ABI for the cluster
target_compile_options(${TEST_NAME}_cluster_static
  PRIVATE
  -O2
)
target_link_libraries(${TEST_NAME}_cluster_static PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

######## CLUSTER Code ##########################################################

# create a custom target which creates all wrapped objects
add_custom_target(${TEST_NAME}_binwrap)

# main cluster code
add_cluster_shared_target(${TEST_NAME}_cluster)
target_sources(${TEST_NAME}_cluster PUBLIC src_cluster/test_cluster.c)

######## TEST Executable #######################################################
add_chimera_test(
  ${TEST_NAME}
)

# WIESEP: Link the host and cluster code to the test executable
target_link_libraries(${TEST_NAME} PUBLIC ${TEST_NAME}_host)
target_link_libraries(${TEST_NAME} PUBLIC ${TEST_NAME}_cluster_static)
target_link_libraries(${TEST_NAME} PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_wrapped.o")

add_dependencies(${TEST_NAME} ${TEST_NAME}_binwrap)
