# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

# Define mappings for tests
set(TEST_MAPPINGS
[[
chimera-convolve:
  chimera-convolve,
  host/alloc,
  host/picolibc,
  host/printf,
  host/returnZero,
  host/uartSimple,
  snitchCluster/matmul,
  snitchCluster/snrt,
  snitchCluster/simpleOffload,
  snitchCluster/dynamic
]]
[[
chimera-open:
  chimera-open,
  host/alloc,
  host/picolibc,
  host/printf,
  host/returnZero,
  host/uartSimple,
  snitchCluster/matmul,
  snitchCluster/snrt,
  snitchCluster/simpleOffload,
  snitchCluster/doublelib,
  snitchCluster/dynamic,
  snitchCluster/dynlibc,
  snitchCluster/tls
]]
[[
chimera-host:
  chimera-host,
  host/alloc,
  host/picolibc,
  host/printf,
  host/returnZero,
  host/uartSimple
]]
)

# idk if this should be here
function(add_cluster_shared_target name)
add_library(${name} SHARED)
# this causes a different minimal linkerscript to be used (see targets/chimera-open cmake list)
set_target_properties(${name} PROPERTIES LINK_DYNAMIC TRUE)
# this is necessary because clang does not "support" shared libraries on this platform
# so we need to tell the linker directly to do it (-Xlinker passes the next argument to the linker)
target_link_options(${name} PRIVATE -Xlinker -shared)
target_include_directories(${name} PUBLIC include)
target_link_libraries(${name} PUBLIC runtime_cluster_snitch)
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


# Call the macro
add_chimera_subdirectories(${TARGET_PLATFORM} "Test" "${TEST_MAPPINGS}")
