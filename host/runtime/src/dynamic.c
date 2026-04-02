
#include "dynamic.h"
#include "elf.h"
#include "log.h"
#include <stdint.h>
#include <string.h>

int load_so(struct dyn_loaded* dyns, unsigned char* so_start) {

    printf_log("Binary starts at: %p\n", so_start);

    Elf32_Ehdr* header = (Elf32_Ehdr*) so_start;

    if (header->e_type != ET_DYN) {
        printf_log("ELF is of wrong type, expected: %i, got: %i!\n", ET_DYN, header->e_type);
        return 0;
    } else {
        printf_log("ELF is a dynamic library.\n");
    }

    if (header->e_machine != EM_RISCV) {
        printf_log("ELF is for wrong machine, expected: %i, got: %i!\n", EM_RISCV, header->e_machine);
        return 0;
    } else {
        printf_log("ELF is for RISC-V.\n");
    }

    printf_log("ELF program header table starts at offset %x, has %i entries of size %i.\n", header->e_phoff, header->e_phnum, header->e_phentsize);

    // determine required space
    Elf32_Addr load_size = 0;
    for (uint16_t i = 0; i < header->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(so_start + header->e_phoff + i*header->e_phentsize);
        if (phdr->p_type == PT_LOAD) {
            Elf32_Addr addr_end = phdr->p_vaddr + phdr->p_memsz;
            if (addr_end > load_size) load_size = addr_end;
        }
    }

    printf_log("ELF will require %x bytes of memory when loaded\n", load_size);
    void* load_start = memory_island_malloc(load_size);
    dyns->load_start = load_start;
    printf_log("ELF will be loaded starting at %p\n", load_start);

    // load stuff and setup pointer to dynamic section
    Elf32_Dyn* dyn_start = NULL;
    for (uint16_t i = 0; i < header->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(so_start + header->e_phoff + i*header->e_phentsize);
        if (phdr->p_type == PT_LOAD) {
            printf_log("Copying %x bytes from %p to %p\n", phdr->p_filesz, so_start + phdr->p_offset, load_start + phdr->p_vaddr);
            memcpy(load_start + phdr->p_vaddr, so_start + phdr->p_offset, phdr->p_filesz);
        } else if (phdr->p_type == PT_DYNAMIC) {
            dyn_start = load_start + phdr->p_vaddr;
        }
    }

    if (dyn_start == NULL) {
        printf_log("Dynamic section not found!\n");
        return 1;
    }


    // struct dt_entries* dte = memory_island_malloc(sizeof(struct dt_entries));

    for (Elf32_Dyn* dyn = dyn_start; dyn->d_tag != DT_NULL; dyn++) {
        printf_log("Found dynamic entry with tag: %i\n", dyn->d_tag);
        switch (dyn->d_tag) {
            case DT_RELA:
                dyns->rela = load_start + dyn->d_un.d_ptr;
                break;
            case DT_RELASZ:
                dyns->rela_cnt = dyn->d_un.d_val / sizeof(Elf32_Rela);
                break;
            case DT_RELAENT:
                if (dyn->d_un.d_val != sizeof(Elf32_Rela)) {
                    printf_log("Error: Rela entries have the wrong size! Expected: %i, got: %i\n", sizeof(Elf32_Rela), dyn->d_un.d_val);
                }
                break;
            case DT_JMPREL:
                dyns->relaplt = load_start + dyn->d_un.d_ptr;
                break;
            case DT_PLTRELSZ:
                dyns->relaplt_cnt = dyn->d_un.d_val / sizeof(Elf32_Rela);
                break;
            case DT_PLTGOT:
                // todo: this
                break;
            case DT_SYMTAB:
                dyns->symtab = load_start + dyn->d_un.d_ptr;
                break;
            case DT_SYMENT:
                if (dyn->d_un.d_val != sizeof(Elf32_Sym)) {
                    printf_log("Error: Sym entries have the wrong size! Expected: %i, got: %i\n", sizeof(Elf32_Sym), dyn->d_un.d_val);
                }
                break;
            case DT_STRTAB:
                dyns->strtab = load_start + dyn->d_un.d_ptr;
                break;
            case DT_STRSZ:
                // todo
                break;
            case DT_HASH:
                dyns->hash = load_start + dyn->d_un.d_ptr;
                break;
        }
    }

    return load_size;
}

uint32_t elf_hash(const unsigned char* name) {
    // https://gabi.xinuos.com/elf/08-dynamic.html#id12
    uint32_t h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        if ((g = h) & 0xf0000000) {
            h ^= g >> 24;
        }
        h &= -g;
    }
    return h;
}

Elf32_Sym* locate_symbol(const struct dyn_loaded* dyns, const char* name) {
    uint32_t hash = elf_hash((unsigned char*)name);
    printf_log("hash %s->%u\n",name, hash);

    Elf32_Word nbucket = *dyns->hash;
    Elf32_Word nchain = *(dyns->hash + 1);

    uint16_t ci = *(dyns->hash + 2 + hash%nbucket);

    Elf32_Sym* sym;
    do {
        sym = dyns->symtab + ci;
        char* symname = dyns->strtab + sym->st_name;
        printf_log("ci=%u, name=%s\n", ci, symname);
        if (strcmp(symname, name) == 0) {
            return sym;
        }
        ci = *(dyns->hash + 2 + nbucket + ci);
    } while (!(sym->st_name == 0 &&
        sym->st_value == 0 &&
        sym->st_size == 0 &&
        sym->st_info == 0 &&
        sym->st_other == 0 &&
        sym->st_shndx == SHN_UNDEF));
    
    // symbol not found
    return NULL;
}

void* get_symbol_pointer(const struct dyn_loaded* dyns, const char* name) {
    // assuming the symbol value is referring to an address
    Elf32_Sym* sym = locate_symbol(dyns, name);
    return dyns->load_start + sym->st_value;
}

void print_sym_info(const struct dyn_loaded* dyns, const char* name) {
    Elf32_Sym* sym = locate_symbol(dyns, name);
    printf_log("name: %s, value: %x, size: %x, info: %x\n", dyns->strtab + sym->st_name, sym->st_value, sym->st_size, sym->st_info);
}

void print_reloc_sym(const struct dyn_loaded* dyns) {
    for (uint16_t i = 0; i < dyns->rela_cnt; ++i) {
        Elf32_Rela* rela = dyns->rela + i;
        Elf32_Addr r_offset = rela->r_offset;
        uint32_t r_sym = ELF32_R_SYM(rela->r_info);
        Elf32_Sym* sym = dyns->symtab + r_sym;
        printf_log("  offset: %x, info: %x, name: %s, value: %x, size: %x, info: %x\n", r_offset, rela->r_info, dyns->strtab + sym->st_name, sym->st_value, sym->st_size, sym->st_info);
    }
}

void relocate_single_symbol(const struct dyn_loaded* dyn_main, const struct dyn_loaded* dyn_provider, const char* name) {
    // this is hardcoded to do a RISCV_32 relocation for now and ignores the addend (shitty)
    for (uint16_t i = 0; i < dyn_main->rela_cnt; ++i) {
        Elf32_Rela* rela = dyn_main->rela + i;
        uint32_t r_sym = ELF32_R_SYM(rela->r_info);
        Elf32_Sym* sym = dyn_main->symtab + r_sym;
        char* symname = dyn_main->strtab + sym->st_name;
        printf_log("comparing: name=%s, symname=%s\n", name, symname);
        if (strcmp(name, symname) == 0) {
            printf_log("Found relocation entry for symbol '%s'\n", symname);
            Elf32_Sym* sym_prov = locate_symbol(dyn_provider, name);
            if (sym_prov != NULL) {
                uint32_t** dest = dyn_main->load_start + rela->r_offset;
                printf_log("before relocation: %p->%p\n", dest, *dest);
                *dest = dyn_provider->load_start + sym_prov->st_value;
                printf_log("after: %p->%p\n", dest, *dest);
                return;
            } else {
                printf_log("Symbol not provided in specified file!\n");
                return;
            }
        }
    }
    printf_log("Symbol not found in relocation table!\n");
}