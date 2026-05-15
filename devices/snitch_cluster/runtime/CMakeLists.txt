# SPDX-FileCopyrightText: 2024 ETH Zurich and University of Bologna
# SPDX-License-Identifier: Apache-2.0

################################################################################
# Snitch Cluster Runtime Library                                               #
################################################################################
file(GLOB C_SOURCES
  "putc.c"
  "init.c"
)

target_sources(runtime_cluster_snitch PRIVATE ${C_SOURCES})
target_compile_definitions(runtime_cluster_snitch PUBLIC CHIMERA_DEVICE_SNITCH_CLUSTER)

target_include_directories(runtime_cluster_snitch PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(runtime_cluster_snitch_pic PRIVATE ${C_SOURCES})
target_compile_definitions(runtime_cluster_snitch_pic PUBLIC CHIMERA_DEVICE_SNITCH_CLUSTER)
target_compile_definitions(runtime_cluster_snitch_pic PUBLIC SNRT_SHARED_LIBRARY)

target_include_directories(runtime_cluster_snitch_pic PUBLIC ${CMAKE_CURRENT_LIST_DIR})
