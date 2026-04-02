

// Include Application Headers
// #include "test_cluster.h"

// Include Target Specific Headers
#include "alloc.h"
#include "elf.h"
#include "soc.h"

// Include Driver Headers
#include "driver.h"

// Include Runtime Headers
#include "log.h"
#include "dynamic.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define CLUSTER1 0
#define STACK_ADDRESS (_chimera_clusterBase[CLUSTER1] + 0x20000 - 1)

extern void* __global_pointer$;
extern void* __base_clint;

extern unsigned char _binary_test_cluster_so_start[];
extern unsigned char _binary_test_cluster_so_end[];
extern unsigned char _binary_test_cluster_so_size[];

extern unsigned char _binary_cluster_ret1_so_start[];
extern unsigned char _binary_cluster_ret1_so_end[];
extern unsigned char _binary_cluster_ret1_so_size[];

int main(void) {

    printf_log("Hello!\n");

    printf_log("loading main so\n");
    struct dyn_loaded* dyns_main = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_main, _binary_test_cluster_so_start);

    printf_log("loading secondary so\n");
    struct dyn_loaded* dyns_prov = memory_island_malloc(sizeof(struct dyn_loaded));
    load_so(dyns_prov, _binary_cluster_ret1_so_start);

    uint32_t (*getlibvar)(void) = get_symbol_pointer(dyns_main, "getLibraryVariant");

    uint32_t lib_var = getlibvar();
    uint32_t* lib_var_p = get_symbol_pointer(dyns_main, "libraryVariant");
    printf_log("before relocation: getLibraryVariant()->%u, libraryVariant points to %p\n", lib_var, lib_var_p);
    relocate_single_symbol(dyns_main, dyns_prov, "libraryVariant");
    lib_var = getlibvar();
    lib_var_p = get_symbol_pointer(dyns_main, "libraryVariant");
    printf_log("after relocation: getLibraryVariant()->%u, libraryVariant points to %p\n", lib_var, lib_var_p);

    // printf_log("Symbols in the relocation section:\n");
    // print_reloc_sym(dyns_main);

    //print_sym_info(dyns_main, "clusterEntry");


    return 0;
}
