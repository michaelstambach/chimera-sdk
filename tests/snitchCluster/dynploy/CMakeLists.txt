# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

set(TEST_NAME test_snitchCluster_dynploy)

######## HOST Code #############################################################

# static
add_library(${TEST_NAME}_host_static OBJECT src_host/test_host.c src_host/func_static.c)
target_include_directories(${TEST_NAME}_host_static PUBLIC include)
target_compile_options(${TEST_NAME}_host_static PRIVATE -O2)
target_link_libraries(${TEST_NAME}_host_static PUBLIC runtime_host picolibc_host trampoline_rv32im)

# dynamic
add_library(${TEST_NAME}_host_dyn OBJECT src_host/test_host.c src_host/func_dyn.c)
target_include_directories(${TEST_NAME}_host_dyn PUBLIC include)
target_compile_options(${TEST_NAME}_host_dyn PRIVATE -O2)
target_link_libraries(${TEST_NAME}_host_dyn PUBLIC runtime_host picolibc_host trampoline_rv32im)

######## static CLUSTER Code ####################################################
# cluster code that is always statically linked
add_library(${TEST_NAME}_cluster_static OBJECT src_cluster/cluster_static.c)
target_include_directories(${TEST_NAME}_cluster_static PUBLIC include)

# WIESEP: Set the correct ISA and ABI for the cluster
target_compile_options(${TEST_NAME}_cluster_static
  PRIVATE
  -O2
)
target_link_libraries(${TEST_NAME}_cluster_static PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

# cluster work variant 1
add_library(${TEST_NAME}_cluster_static_work1 OBJECT
  src_cluster/main.c
  src_cluster/Network.c
  src_cluster/deeploysnitchlib/src/Gemm_s8.c
)
target_include_directories(${TEST_NAME}_cluster_static_work1 PUBLIC
  include
  src_cluster/inc
  src_cluster/deeploysnitchlib/inc
)
target_compile_options(${TEST_NAME}_cluster_static_work1 PRIVATE -O2)
target_link_libraries(${TEST_NAME}_cluster_static_work1 PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)
# cluster work variant 2
# add_library(${TEST_NAME}_cluster_static_work2 OBJECT src_cluster/test_cluster.c src_cluster/cluster_work2.c)
# target_include_directories(${TEST_NAME}_cluster_static_work2 PUBLIC include)
# target_compile_options(${TEST_NAME}_cluster_static_work2 PRIVATE -O2)
# target_link_libraries(${TEST_NAME}_cluster_static_work2 PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

######## dynamic CLUSTER Code ###################################################

# create a custom target which creates all wrapped objects
add_custom_target(${TEST_NAME}_binwrap)

# main cluster code and network
add_cluster_shared_target(${TEST_NAME}_cluster_main)
target_sources(${TEST_NAME}_cluster_main PUBLIC src_cluster/main.c src_cluster/Network.c)
target_include_directories(${TEST_NAME}_cluster_main PUBLIC include src_cluster/inc src_cluster/deeploysnitchlib/inc)
# target_compile_definitions(${TEST_NAME}_cluster_main PUBLIC _PICOLIBC_PRINTF=i)
# target_link_options(${TEST_NAME}_cluster_main PUBLIC "SHELL: -Xlinker --defsym=vfprintf=__i_vfprintf")
# currently integrated into main to reduce tls headache
# # network
# add_cluster_shared_target(${TEST_NAME}_cluster_network)
# target_sources(${TEST_NAME}_cluster_network PUBLIC src_cluster/Network.c)
# target_include_directories(${TEST_NAME}_cluster_network PUBLIC src_cluster/inc src_cluster/deeploysnitchlib/inc)
# core
add_cluster_shared_target(${TEST_NAME}_cluster_gemm)
target_sources(${TEST_NAME}_cluster_gemm PUBLIC src_cluster/deeploysnitchlib/src/Gemm_s8.c)
target_include_directories(${TEST_NAME}_cluster_gemm PUBLIC src_cluster/deeploysnitchlib/inc)

######## TEST Executable #######################################################
# statically linked
add_chimera_test(${TEST_NAME}_static1)
target_link_libraries(${TEST_NAME}_static1 PUBLIC ${TEST_NAME}_host_static)
target_link_libraries(${TEST_NAME}_static1 PUBLIC ${TEST_NAME}_cluster_static)
target_link_libraries(${TEST_NAME}_static1 PUBLIC ${TEST_NAME}_cluster_static_work1)

# add_chimera_test(${TEST_NAME}_static2)
# target_link_libraries(${TEST_NAME}_static2 PUBLIC ${TEST_NAME}_host_static)
# target_link_libraries(${TEST_NAME}_static2 PUBLIC ${TEST_NAME}_cluster_static)
# target_link_libraries(${TEST_NAME}_static2 PUBLIC ${TEST_NAME}_cluster_static_work2)

# fully dynamic
add_chimera_test(${TEST_NAME}_dyn)
target_link_libraries(${TEST_NAME}_dyn PUBLIC ${TEST_NAME}_host_dyn)
target_link_libraries(${TEST_NAME}_dyn PUBLIC ${TEST_NAME}_cluster_static)
target_link_libraries(${TEST_NAME}_dyn PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_main_wrapped.o")
target_link_libraries(${TEST_NAME}_dyn PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_gemm_wrapped.o")
add_dependencies(${TEST_NAME}_dyn ${TEST_NAME}_binwrap)
