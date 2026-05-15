
#include "addr_maps/soc_addr_map.h"
#include "alloc.h"
#include "cluster_static.h"
#include "log.h"
#include "dynamic.h"
#include "test_func.h"
#include <stdint.h>

extern unsigned char _binary_test_snitchCluster_dynploy_cluster_main_start[];
extern unsigned char _binary_test_snitchCluster_dynploy_cluster_main_end[];
extern unsigned char _binary_test_snitchCluster_dynploy_cluster_main_size[];

extern unsigned char _binary_test_snitchCluster_dynploy_cluster_gemm_start[];
extern unsigned char _binary_test_snitchCluster_dynploy_cluster_gemm_end[];
extern unsigned char _binary_test_snitchCluster_dynploy_cluster_gemm_size[];

// extern uintptr_t volatile tohost, fromhost;

int32_t (*get_function_pointer(void** stack_ptr))(void* args) {
    printf_log("loading cluster main\n");
    struct dyn_loaded* dyns_main = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_main, _binary_test_snitchCluster_dynploy_cluster_main_start);

    printf_log("loading cluster kernel\n");
    struct dyn_loaded* dyns_gemm = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_gemm, _binary_test_snitchCluster_dynploy_cluster_gemm_start);

    printf_log("performing self-relocations for main library\n");
    attempt_relocations(dyns_main, dyns_main);

    printf_log("relocating kernel into main lib\n");
    attempt_relocations(dyns_main, dyns_gemm);

    printf_log("manually relocating special symbols\n");
    relocate_single_symbol(dyns_main, 0, "_chimera_clusterHeapStart", (void *)_chimera_clusterHeapStart);
    // relocate_single_symbol(dyns_main, 0, "stdout", (void *)stdout);
    // relocate_single_symbol(dyns_main, 0, "tohost", (void *)tohost);
    // relocate_single_symbol(dyns_main, 0, "fromhost", (void *)fromhost);
    relocate_single_symbol(dyns_main, 1, "__tls_get_addr", (void *)__tls_get_addr);

    printf_log("retrieving pointer to entry function\n");
    int32_t (*clusterMain)(void* args) = get_symbol_pointer(dyns_main, "clusterMain");

    printf_log("adding tls section for the dm core\n");
    // initialise with 0
    *(uint32_t *)*stack_ptr = 0;
    add_tls_module(dyns_main, stack_ptr);

    return clusterMain;
}
