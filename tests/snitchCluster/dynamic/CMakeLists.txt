# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

set(TEST_NAME test_snitchCluster_dynamic)

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

#### statically link cluster interrupt handler

add_library(${TEST_NAME}_cluster_interrupt OBJECT src_cluster/interrupt_handler.c)
target_include_directories(${TEST_NAME}_cluster_interrupt PUBLIC include)

# WIESEP: Set the correct ISA and ABI for the cluster
target_compile_options(${TEST_NAME}_cluster_interrupt
  PRIVATE
  -O2
)
target_link_libraries(${TEST_NAME}_cluster_interrupt PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)

######## CLUSTER Code ##########################################################

# create a custom target which creates all wrapped objects
add_custom_target(${TEST_NAME}_binwrap)

function(add_cluster_shared_target name)
add_library(${name} SHARED)
# this causes a different minimal linkerscript to be used (see targets/chimera-open cmake list)
set_target_properties(${name} PROPERTIES LINK_DYNAMIC TRUE)
# this is necessary because clang does not "support" shared libraries on this platform
# so we need to tell the linker directly to do it (-Xlinker passes the next argument to the linker)
target_link_options(${name} PRIVATE -Xlinker -shared)
target_include_directories(${name} PUBLIC include)
target_link_libraries(${name} PUBLIC runtime_cluster_snitch picolibc_cluster_snitch)
# wrap the whole thing into the data section
set(LIB_SO_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${name}.so")
set(LIB_WRAPPED_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${name}_wrapped.o")
string(REGEX REPLACE [-/\.] _ LIB_SYMBOL_PREFIX "_binary_${LIB_SO_PATH}")
add_custom_command(OUTPUT "${LIB_WRAPPED_PATH}"
  DEPENDS ${name}
  COMMAND ${CMAKE_OBJCOPY} -I binary -O elf32-littleriscv "${LIB_SO_PATH}" "${LIB_WRAPPED_PATH}"
  # give the symbols some more normal names
  COMMAND ${CMAKE_OBJCOPY} --redefine-sym ${LIB_SYMBOL_PREFIX}_start=_binary_${name}_start --redefine-sym ${LIB_SYMBOL_PREFIX}_end=_binary_${name}_end --redefine-sym ${LIB_SYMBOL_PREFIX}_size=_binary_${name}_size ${LIB_WRAPPED_PATH} ${LIB_WRAPPED_PATH}
)
add_custom_target(${name}_binwrap DEPENDS "${LIB_WRAPPED_PATH}")
add_dependencies(${TEST_NAME}_binwrap ${name}_binwrap)
endfunction()

# main cluster code
add_cluster_shared_target(${TEST_NAME}_cluster)
target_sources(${TEST_NAME}_cluster PUBLIC src_cluster/test_cluster.c)

# cluster work 1
add_cluster_shared_target(${TEST_NAME}_cluster_work1)
target_sources(${TEST_NAME}_cluster_work1 PUBLIC src_cluster/cluster_ret1.c)

# cluster work 2
add_cluster_shared_target(${TEST_NAME}_cluster_work2)
target_sources(${TEST_NAME}_cluster_work2 PUBLIC src_cluster/cluster_ret2.c)

######## TEST Executable #######################################################
add_chimera_test(
  ${TEST_NAME}
)

# WIESEP: Link the host and cluster code to the test executable
target_link_libraries(${TEST_NAME} PUBLIC ${TEST_NAME}_host)
target_link_libraries(${TEST_NAME} PUBLIC ${TEST_NAME}_cluster_interrupt)
target_link_libraries(${TEST_NAME} PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_wrapped.o")
target_link_libraries(${TEST_NAME} PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_work1_wrapped.o")
target_link_libraries(${TEST_NAME} PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${TEST_NAME}_cluster_work2_wrapped.o")

add_dependencies(${TEST_NAME} ${TEST_NAME}_binwrap)
