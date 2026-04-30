# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

set(TEST_NAME test_snitchCluster_dynlibc)

# get_property(SHARED_SUPPORT GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
# message("supports shared libs: ${SHARED_SUPPORT}")

######## HOST Code #############################################################
# this is the same for all variants
file(GLOB_RECURSE TEST_HOST_SRCS
  "src_host/*.c"
)

# static
add_library(${TEST_NAME}_host_static OBJECT ${TEST_HOST_SRCS})
target_include_directories(${TEST_NAME}_host_static PUBLIC include)
target_compile_definitions(${TEST_NAME}_host_static PRIVATE VARIANT_STATIC)
target_compile_options(${TEST_NAME}_host_static PRIVATE -O2)
target_link_libraries(${TEST_NAME}_host_static PUBLIC runtime_host picolibc_host trampoline_rv32im)

# mixed
add_library(${TEST_NAME}_host_mixed OBJECT ${TEST_HOST_SRCS})
target_include_directories(${TEST_NAME}_host_mixed PUBLIC include)
target_compile_definitions(${TEST_NAME}_host_mixed PRIVATE VARIANT_MIXED)
target_compile_options(${TEST_NAME}_host_mixed PRIVATE -O2)
target_link_libraries(${TEST_NAME}_host_mixed PUBLIC runtime_host picolibc_host trampoline_rv32im)

# dynamic
add_library(${TEST_NAME}_host_dyn OBJECT ${TEST_HOST_SRCS})
target_include_directories(${TEST_NAME}_host_dyn PUBLIC include)
target_compile_definitions(${TEST_NAME}_host_dyn PRIVATE VARIANT_DYN)
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
add_library(${TEST_NAME}_cluster_static_work1 OBJECT src_cluster/test_cluster.c src_cluster/cluster_work1.c)
target_include_directories(${TEST_NAME}_cluster_static_work1 PUBLIC include)
target_compile_options(${TEST_NAME}_cluster_static_work1 PRIVATE -O2)
target_link_libraries(${TEST_NAME}_cluster_static_work1 PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)
# cluster work variant 2
add_library(${TEST_NAME}_cluster_static_work2 OBJECT src_cluster/test_cluster.c src_cluster/cluster_work2.c)
target_include_directories(${TEST_NAME}_cluster_static_work2 PUBLIC include)
target_compile_options(${TEST_NAME}_cluster_static_work2 PRIVATE -O2)
target_link_libraries(${TEST_NAME}_cluster_static_work2 PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

######## dynamic CLUSTER Code ###################################################

# create a custom target which creates all wrapped objects
add_custom_target(${TEST_NAME}_binwrap)

# main cluster code
add_cluster_shared_target(${TEST_NAME}_cluster)
target_sources(${TEST_NAME}_cluster PUBLIC src_cluster/test_cluster.c)

## mixed version (static libc)
# cluster work variant 1: make a string lowercase
add_cluster_shared_target(${TEST_NAME}_cluster_work1_mixed)
target_sources(${TEST_NAME}_cluster_work1_mixed PUBLIC src_cluster/cluster_work1.c)
# cluster work variant 2: make a string lowercase and reverse it
add_cluster_shared_target(${TEST_NAME}_cluster_work2_mixed)
target_sources(${TEST_NAME}_cluster_work2_mixed PUBLIC src_cluster/cluster_work2.c)
# statically link libc (note the use of picolibc_host to match ABI)
target_link_libraries(${TEST_NAME}_cluster_work1_mixed PUBLIC picolibc_host)
target_link_libraries(${TEST_NAME}_cluster_work2_mixed PUBLIC picolibc_host)

## fully dynamic version (libc also dynamic)
# cluster work variant 1: make a string lowercase
add_cluster_shared_target(${TEST_NAME}_cluster_work1_dyn)
target_sources(${TEST_NAME}_cluster_work1_dyn PUBLIC src_cluster/cluster_work1.c)
# cluster work variant 2: make a string lowercase and reverse it
add_cluster_shared_target(${TEST_NAME}_cluster_work2_dyn)
target_sources(${TEST_NAME}_cluster_work2_dyn PUBLIC src_cluster/cluster_work2.c)
# provide libc dynamically as well
add_cluster_shared_target(${TEST_NAME}_cluster_libc_dyn)
# TEMPORARY: place tolower.c here manually from picolibc
target_sources(${TEST_NAME}_cluster_libc_dyn PUBLIC src_cluster/tolower.c)
#  with this picolibc does not require a table for character comparison
#  better for size and good enough for us, we do not need locales :)
target_compile_definitions(${TEST_NAME}_cluster_libc_dyn PRIVATE _PICOLIBC_CTYPE_SMALL=1)

######## TEST Executable #######################################################
# statically linked
add_chimera_test(${TEST_NAME}_static1)
target_link_libraries(${TEST_NAME}_static1 PUBLIC ${TEST_NAME}_host_static)
target_link_libraries(${TEST_NAME}_static1 PUBLIC ${TEST_NAME}_cluster_static)
target_link_libraries(${TEST_NAME}_static1 PUBLIC ${TEST_NAME}_cluster_static_work1)

add_chimera_test(${TEST_NAME}_static2)
target_link_libraries(${TEST_NAME}_static2 PUBLIC ${TEST_NAME}_host_static)
target_link_libraries(${TEST_NAME}_static2 PUBLIC ${TEST_NAME}_cluster_static)
target_link_libraries(${TEST_NAME}_static2 PUBLIC ${TEST_NAME}_cluster_static_work2)

# mixed (cluster dynamic with static libc)
add_chimera_test(${TEST_NAME}_mixed)
target_link_libraries(${TEST_NAME}_mixed PUBLIC ${TEST_NAME}_host_mixed)
target_link_libraries(${TEST_NAME}_mixed PUBLIC ${TEST_NAME}_cluster_static)
target_link_libraries(${TEST_NAME}_mixed PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_wrapped.o")
target_link_libraries(${TEST_NAME}_mixed PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_work1_mixed_wrapped.o")
target_link_libraries(${TEST_NAME}_mixed PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_work2_mixed_wrapped.o")
add_dependencies(${TEST_NAME}_mixed ${TEST_NAME}_binwrap)

# fully dynamic
add_chimera_test(${TEST_NAME}_dyn)
target_link_libraries(${TEST_NAME}_dyn PUBLIC ${TEST_NAME}_host_dyn)
target_link_libraries(${TEST_NAME}_dyn PUBLIC ${TEST_NAME}_cluster_static)
target_link_libraries(${TEST_NAME}_dyn PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_wrapped.o")
target_link_libraries(${TEST_NAME}_dyn PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_work1_dyn_wrapped.o")
target_link_libraries(${TEST_NAME}_dyn PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_work2_dyn_wrapped.o")
target_link_libraries(${TEST_NAME}_dyn PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_libc_dyn_wrapped.o")
add_dependencies(${TEST_NAME}_dyn ${TEST_NAME}_binwrap)
