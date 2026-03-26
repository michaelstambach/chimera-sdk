

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
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define CLUSTER1 0
#define STACK_ADDRESS (_chimera_clusterBase[CLUSTER1] + 0x20000 - 1)

extern unsigned char _binary_test_cluster_so_start[];
extern unsigned char _binary_test_cluster_so_end[];
extern unsigned char _binary_test_cluster_so_size[];

struct dt_entries {
    // these are just the entries which my sample file has for now
    Elf32_Addr rela;
    Elf32_Word relasz;
    Elf32_Word relaent;
    Elf32_Addr jmprel;
    Elf32_Word pltrelsz;
    Elf32_Addr pltgot;
    Elf32_Addr symtab;
    Elf32_Word syment;
    Elf32_Addr strtab;
    Elf32_Word strsz;
    Elf32_Addr hash;
};

int main(void) {

    printf_log("Hello!\n");

    printf_log("Binary starts at: %p\n", _binary_test_cluster_so_start);

    void* elf_start = _binary_test_cluster_so_start;

    Elf32_Ehdr* header = (Elf32_Ehdr*) elf_start;

    if (header->e_type != ET_DYN) {
        printf_log("ELF is of wrong type, expected: %i, got: %i!\n", ET_DYN, header->e_type);
        return 1;
    } else {
        printf_log("ELF is a dynamic library.\n");
    }

    if (header->e_machine != EM_RISCV) {
        printf_log("ELF is for wrong machine, expected: %i, got: %i!\n", EM_RISCV, header->e_machine);
        return 1;
    } else {
        printf_log("ELF is for RISC-V.\n");
    }

    printf_log("ELF program header table starts at offset %x, has %i entries of size %i.\n", header->e_phoff, header->e_phnum, header->e_phentsize);

    // determine required space
    Elf32_Addr load_size = 0;
    for (uint16_t i = 0; i < header->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(_binary_test_cluster_so_start + header->e_phoff + i*header->e_phentsize);
        if (phdr->p_type == PT_LOAD) {
            Elf32_Addr addr_end = phdr->p_vaddr + phdr->p_memsz;
            if (addr_end > load_size) load_size = addr_end;
        }
    }

    printf_log("ELF will require %x bytes of memory when loaded\n", load_size);
    void* load_start = memory_island_malloc(load_size);
    printf_log("ELF will be loaded starting at %p\n", load_start);

    // load stuff and setup pointer to dynamic section
    Elf32_Dyn* dyn_start = NULL;
    for (uint16_t i = 0; i < header->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(_binary_test_cluster_so_start + header->e_phoff + i*header->e_phentsize);
        if (phdr->p_type == PT_LOAD) {
            printf_log("Copying %x bytes from %p to %p\n", phdr->p_filesz, elf_start + phdr->p_offset, load_start + phdr->p_vaddr);
            memcpy(load_start + phdr->p_vaddr, elf_start + phdr->p_offset, phdr->p_filesz);
        } else if (phdr->p_type == PT_DYNAMIC) {
            dyn_start = load_start + phdr->p_vaddr;
        }
    }

    if (dyn_start == NULL) {
        printf_log("Dynamic section not found!\n");
        return 1;
    }


    struct dt_entries* dte = memory_island_malloc(sizeof(struct dt_entries));

    for (Elf32_Dyn* dyn = dyn_start; dyn->d_tag != DT_NULL; dyn++) {
        printf_log("Found dynamic entry with tag: %i\n", dyn->d_tag);
        switch (dyn->d_tag) {
            case DT_RELA:
                dte->rela = dyn->d_un.d_ptr;
                break;
            case DT_RELASZ:
                dte->relasz = dyn->d_un.d_val;
                break;
            case DT_RELAENT:
                dte->relaent = dyn->d_un.d_val;
                break;
            case DT_JMPREL:
                dte->jmprel = dyn->d_un.d_ptr;
                break;
            case DT_PLTRELSZ:
                dte->pltrelsz = dyn->d_un.d_val;
                break;
            case DT_PLTGOT:
                dte->pltgot = dyn->d_un.d_ptr;
                break;
            case DT_SYMTAB:
                dte->symtab = dyn->d_un.d_ptr;
                break;
            case DT_SYMENT:
                dte->syment = dyn->d_un.d_val;
                break;
            case DT_STRTAB:
                dte->strtab = dyn->d_un.d_ptr;
                break;
            case DT_STRSZ:
                dte->strsz = dyn->d_un.d_val;
                break;
            case DT_HASH:
                dte->hash = dyn->d_un.d_ptr;
                break;
        }
    }

    uint16_t rela_cnt = dte->relasz / dte->relaent;
    uint16_t relaplt_cnt = dte->pltrelsz / dte->relaent;
    Elf32_Sym* sym_start = load_start + dte->symtab;
    char* str_start = load_start + dte->strtab;

    printf_log("Symbols in the relocation section:\n");
    for (uint16_t i = 0; i < rela_cnt; ++i) {
        Elf32_Rela* rela = load_start + dte->rela + (i * dte->relaent);
        Elf32_Addr r_offset = rela->r_offset;
        uint32_t r_sym = ELF32_R_SYM(rela->r_info);
        Elf32_Sym* sym = sym_start + r_sym;
        printf_log("  offset: %x, info: %x, name: %s, value: %x, size: %x, info: %x\n", r_offset, rela->r_info, str_start + sym->st_name, sym->st_value, sym->st_size, sym->st_info);
    }

    printf_log("Symbols in the PLT relocation section:\n");
    for (uint16_t i = 0; i < relaplt_cnt; ++i) {
        Elf32_Rela* rela = load_start + dte->jmprel + (i * dte->relaent);
        Elf32_Addr r_offset = rela->r_offset;
        uint32_t r_sym = ELF32_R_SYM(rela->r_info);
        Elf32_Sym* sym = sym_start + r_sym;
        printf_log("  offset: %x, info: %x, name: %s, value: %x, size: %x, info: %x\n", r_offset, rela->r_info, str_start + sym->st_name, sym->st_value, sym->st_size, sym->st_info);
    }

    return 0;
}
