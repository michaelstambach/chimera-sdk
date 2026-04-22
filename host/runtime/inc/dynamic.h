#include "alloc.h"
#include "soc.h"

#include "log.h"

#include "elf.h"

#include <stdint.h>
#include <string.h>

struct dyn_loaded {
    void* load_start;
    Elf32_Rela* rela;
    uint16_t rela_cnt;
    Elf32_Rela* relaplt;
    uint16_t relaplt_cnt;
    Elf32_Sym* symtab;
    char* strtab;
    Elf32_Word* hash;
    Elf32_Phdr* tls_phdr;
    uint32_t tls_module_id;
};

int load_so(struct dyn_loaded* dyn, unsigned char* so_start);
void load_self(struct dyn_loaded* dyn);

Elf32_Sym* locate_symbol(const struct dyn_loaded* dyns, const char* name);
void* get_symbol_pointer(const struct dyn_loaded* dyns, const char* name);

void print_reloc_sym(const struct dyn_loaded* dyns);
void print_sym_info(const struct dyn_loaded* dyns, const char* name);

void relocate_global_pointer(const struct dyn_loaded* dyn_main);
void relocate_single_symbol(const struct dyn_loaded* dyn_main, bool useplt, const char* symtarget, void* value);
void attempt_relocations(const struct dyn_loaded* dyn_main, const struct dyn_loaded* dyn_provider);
void add_tls_module(const struct dyn_loaded* dyn, void** cluster_stack);
