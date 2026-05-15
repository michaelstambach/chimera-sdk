
#include "dynamic.h"
#include "elf.h"
#include "log.h"
#include "driver.h"
#include "sys/types.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef CHIMERA_DRIVER_DMA
#include "dma/dma.h"
#endif // CHIMERA_DRIVER_DMA

extern void* __global_pointer$;

uint32_t tls_module_count = 0;

// #define DYN_DEBUG_LOGGING

// offset is a pointer to where the binary starts iff it uses relative positions
// otherwise just use null
void read_dynamic_section(struct dyn_loaded* dyns, Elf32_Dyn* dyn_start, void* offset) {
    // reset to known values
    dyns->rela = NULL;
    dyns->rela_cnt = 0;
    dyns->relaplt = NULL;
    dyns->relaplt_cnt = 0;
    dyns->symtab = NULL;
    dyns->strtab = NULL;
    dyns->hash = NULL;
    #ifdef DYN_DEBUG_LOGGING
    printf_log("Loading dynamic entries starting at %p\n", dyn_start);
    #endif // DYN_DEBUG_LOGGING
    for (Elf32_Dyn* dyn = dyn_start; dyn->d_tag != DT_NULL; dyn++) {
        #ifdef DYN_DEBUG_LOGGING
        //printf_log("Found dynamic entry with tag: %i\n", dyn->d_tag);
        #endif // DYN_DEBUG_LOGGING
        switch (dyn->d_tag) {
            case DT_REL:
                printf_log("Warning: found DT_REL entry, this is not supported yet!\n");
                break;
            case DT_RELA:
                if (dyns->rela == NULL) {
                    dyns->rela = offset + dyn->d_un.d_ptr;
                } else {
                    printf_log("Warning: multiple DT_RELA entries found! Currently unsupported, only the first entry is respected!\n");
                }
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
                dyns->relaplt = offset + dyn->d_un.d_ptr;
                break;
            case DT_PLTRELSZ:
                dyns->relaplt_cnt = dyn->d_un.d_val / sizeof(Elf32_Rela);
                break;
            case DT_PLTGOT:
                // todo: this
                break;
            case DT_SYMTAB:
                dyns->symtab = offset + dyn->d_un.d_ptr;
                break;
            case DT_SYMENT:
                if (dyn->d_un.d_val != sizeof(Elf32_Sym)) {
                    printf_log("Error: Sym entries have the wrong size! Expected: %i, got: %i\n", sizeof(Elf32_Sym), dyn->d_un.d_val);
                }
                break;
            case DT_STRTAB:
                dyns->strtab = offset + dyn->d_un.d_ptr;
                break;
            case DT_HASH:
                dyns->hash = offset + dyn->d_un.d_ptr;
                break;
        }
    }
}

int load_so(struct dyn_loaded* dyns, unsigned char* so_start) {

    #ifdef DYN_DEBUG_LOGGING
    printf_log("Binary starts at: %p\n", so_start);
    #endif // DYN_DEBUG_LOGGING

    Elf32_Ehdr* header = (Elf32_Ehdr*) so_start;

    if (header->e_type != ET_DYN) {
        printf_log("ELF is of wrong type, expected: %i, got: %i!\n", ET_DYN, header->e_type);
        return 0;
    } else {
        #ifdef DYN_DEBUG_LOGGING
        printf_log("ELF is a dynamic library.\n");
        #endif // DYN_DEBUG_LOGGING
    }

    if (header->e_machine != EM_RISCV) {
        printf_log("ELF is for wrong machine, expected: %i, got: %i!\n", EM_RISCV, header->e_machine);
        return 0;
    } else {
        #ifdef DYN_DEBUG_LOGGING
        printf_log("ELF is for RISC-V.\n");
        #endif // DYN_DEBUG_LOGGING
    }

    #ifdef DYN_DEBUG_LOGGING
    printf_log("ELF program header table starts at offset %x, has %i entries of size %i.\n", header->e_phoff, header->e_phnum, header->e_phentsize);
    #endif // DYN_DEBUG_LOGGING

    // determine required space
    Elf32_Addr load_size = 0;
    for (uint16_t i = 0; i < header->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(so_start + header->e_phoff + i*header->e_phentsize);
        if (phdr->p_type == PT_LOAD) {
            Elf32_Addr addr_end = phdr->p_vaddr + phdr->p_memsz;
            if (addr_end > load_size) load_size = addr_end;
        }
    }

    #ifdef DYN_DEBUG_LOGGING
    printf_log("ELF will require %x bytes of memory when loaded\n", load_size);
    #endif // DYN_DEBUG_LOGGING
    void* load_start = memory_island_malloc(load_size);
    if (load_start == NULL) {
        printf_log("malloc for loading the ELF failed!\n");
        return 0;
    }
    dyns->load_start = load_start;
    #ifdef DYN_DEBUG_LOGGING
    printf_log("ELF will be loaded starting at %p\n", load_start);
    #ifdef CHIMERA_DRIVER_DMA
    printf_log("DMA will be used for loading\n");
    #endif // CHIMERA_DRIVER_DMA
    #endif // DYN_DEBUG_LOGGING

    // load stuff and setup pointer to dynamic section
    Elf32_Dyn* dyn_start = NULL;
    dyns->tls_phdr = NULL;
    for (uint16_t i = 0; i < header->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(so_start + header->e_phoff + i*header->e_phentsize);
        if (phdr->p_type == PT_LOAD) {
            #ifdef DYN_DEBUG_LOGGING
            printf_log("Copying %x bytes from %p to %p\n", phdr->p_filesz, so_start + phdr->p_offset, load_start + phdr->p_vaddr);
            #endif // DYN_DEBUG_LOGGING
            #ifdef CHIMERA_DRIVER_DMA
            sys_dma_blk_memcpy((uintptr_t)(load_start + phdr->p_vaddr), (uintptr_t)(so_start + phdr->p_offset), phdr->p_filesz, DMA_CONF_DECOUPLE_NONE);
            #else
            memcpy(load_start + phdr->p_vaddr, so_start + phdr->p_offset, phdr->p_filesz);
            #endif // CHIMERA_DRIVER_DMA
        } else if (phdr->p_type == PT_DYNAMIC) {
            dyn_start = load_start + phdr->p_vaddr;
        } else if (phdr->p_type == PT_TLS) {
            if (dyns->tls_phdr != NULL) {
                printf_log("Warning! Multiple TLS program headers found, only one is supported!\n");
                continue;
            }
            dyns->tls_phdr = phdr;
            dyns->tls_module_id = ++tls_module_count;
        }
    }

    if (dyn_start == NULL) {
        printf_log("Dynamic section not found!\n");
        return 1;
    }


    read_dynamic_section(dyns, dyn_start, load_start);
    #ifdef DYN_DEBUG_LOGGING
    printf_log("hash table is at %p\n", dyns->hash);
    #endif // DYN_DEBUG_LOGGING

    return load_size;
}

uint32_t elf_hash(const char* name) {
    // https://gabi.xinuos.com/elf/08-dynamic.html#id12
    uint32_t h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        if ((g = (h & 0xf0000000))) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

Elf32_Sym* locate_symbol(const struct dyn_loaded* dyns, const char* name) {
    uint32_t hash = elf_hash(name);
    #ifdef DYN_DEBUG_LOGGING
    printf_log("hash %s->%u\n",name, hash);
    #endif // DYN_DEBUG_LOGGING

    Elf32_Word nbucket = *dyns->hash;
    Elf32_Word nchain = *(dyns->hash + 1);

    Elf32_Word ci = *(dyns->hash + 2 + (hash%nbucket));
    //printf_log("chain index: %u, address of that: %p, value there: %u\n", hash % nbucket, (dyns->hash + 2 + (hash%nbucket)), ci);

    Elf32_Sym* sym;
    while (ci != STN_UNDEF) {
        sym = dyns->symtab + ci;
        char* symname = dyns->strtab + sym->st_name;
        #ifdef DYN_DEBUG_LOGGING
        printf_log("ci=%u, name=%s\n", ci, symname);
        #endif // DYN_DEBUG_LOGGING
        //printf_log("entry at %p - name: %s, value: %x, size: %x, info: %x\n", sym, symname, sym->st_value, sym->st_size, sym->st_info);
        if (strcmp(symname, name) == 0) {
            return sym;
        }
        ci = *(dyns->hash + 2 + nbucket + ci);
    }
    
    // symbol not found
    return NULL;
}

void* get_symbol_pointer(const struct dyn_loaded* dyns, const char* name) {
    // assuming the symbol value is referring to an address
    Elf32_Sym* sym = locate_symbol(dyns, name);
    if (sym == NULL) return NULL;
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

// there might be a better solution for this in the future
// maybe we can also come up with our own, better global pointer?
void relocate_global_pointer(const struct dyn_loaded* dyn_main) {
    for (uint16_t i = 0; i < dyn_main->rela_cnt; ++i) {
        Elf32_Rela* rela = dyn_main->rela + i;
        uint32_t r_sym = ELF32_R_SYM(rela->r_info);
        Elf32_Sym* sym = dyn_main->symtab + r_sym;
        char* symname = dyn_main->strtab + sym->st_name;
        if (strcmp("__global_pointer$", symname) == 0) {
            #ifdef DYN_DEBUG_LOGGING
            printf_log("Found relocation entry for __global_pointer$ '%s'\n", symname);
            #endif // DYN_DEBUG_LOGGING
            void** dest = dyn_main->load_start + rela->r_offset;
            *dest = &__global_pointer$;
            return;
        }
    }
}

void relocate_single_symbol(const struct dyn_loaded* dyn_main, bool useplt, const char* symtarget, void* value) {
    for (uint16_t i = 0; i < (useplt ? dyn_main->relaplt_cnt : dyn_main->rela_cnt); ++i) {
        Elf32_Rela* rela = (useplt ? dyn_main->relaplt : dyn_main->rela) + i;
        uint32_t r_sym = ELF32_R_SYM(rela->r_info);
        Elf32_Sym* sym = dyn_main->symtab + r_sym;
        char* symname = dyn_main->strtab + sym->st_name;
        if (strcmp(symtarget, symname) == 0) {
            #ifdef DYN_DEBUG_LOGGING
            printf_log("Found relocation entry for '%s'\n", symname);
            #endif // DYN_DEBUG_LOGGING
            void** dest = dyn_main->load_start + rela->r_offset;
            *dest = value;
            return;
        }
    }
    printf_log("Warning: Symbol '%s' not found in relocations!\n", symtarget);

}

// this is hardcoded to do a RISCV_32 relocation for now
void relocate_single_rela(const struct dyn_loaded* dyn_main, const struct dyn_loaded* dyn_provider, const Elf32_Rela* rela_start, const uint16_t rela_count) {
    for (uint16_t i = 0; i < rela_count; ++i) {
        const Elf32_Rela* rela = rela_start + i;
        uint32_t r_type = ELF32_R_TYPE(rela->r_info);
        if (dyn_main == dyn_provider && r_type == R_RISCV_RELATIVE) {
            // handle relative separately and only when relocating to itself
            uint32_t** dest = dyn_main->load_start + rela->r_offset;
            *dest = dyn_main->load_start + rela->r_offset;
            continue;
        }
        uint32_t r_sym = ELF32_R_SYM(rela->r_info);
        Elf32_Sym* sym = dyn_main->symtab + r_sym;
        char* symname = dyn_main->strtab + sym->st_name;
        if (r_type != R_RISCV_32 &&
            r_type != R_RISCV_JUMP_SLOT &&
            r_type != R_RISCV_TLS_DTPMOD32 &&
            r_type != R_RISCV_TLS_DTPREL32) {
            printf_log("Warning: The relocation entry for symbol '%s' is of an unsupported type (%u), skipping.\n", symname, r_type);
            continue;
        }
        #ifdef DYN_DEBUG_LOGGING
        printf_log("Found relocation entry for symbol '%s'\n", symname);
        #endif // DYN_DEBUG_LOGGING
        Elf32_Sym* sym_prov = locate_symbol(dyn_provider, symname);
        // if SHN_UNDEF the symbol is not defined within this file
        if (sym_prov != NULL && sym_prov->st_shndx != SHN_UNDEF) {
            #ifdef DYN_DEBUG_LOGGING
            // printf_log("before relocation: %p->%p\n", dest, *dest);
            #endif // DYN_DEBUG_LOGGING
            void* provider_relative_offset = NULL;
            if (sym_prov->st_shndx != SHN_ABS) {
                // some symbols have absolute values which we should not
                // adjust to the load address of the provider
                provider_relative_offset = dyn_provider->load_start;
            }
            if (r_type == R_RISCV_32) {
                uint32_t** dest = dyn_main->load_start + rela->r_offset;
                *dest = provider_relative_offset + sym_prov->st_value + rela->r_addend;
            } else if (r_type == R_RISCV_JUMP_SLOT) {
                uint32_t** dest = dyn_main->load_start + rela->r_offset;
                *dest = provider_relative_offset + sym_prov->st_value;
            } else if (r_type == R_RISCV_TLS_DTPMOD32) {
                uint32_t* dest = dyn_main->load_start + rela->r_offset;
                *dest = dyn_provider->tls_module_id;
            } else if (r_type == R_RISCV_TLS_DTPREL32) {
                uint32_t* dest = dyn_main->load_start + rela->r_offset;
                *dest = sym_prov->st_value + rela->r_addend;
            } else {
                printf_log("Something has gone wrong, you should never see this!\n");
            }
            #ifdef DYN_DEBUG_LOGGING
            // printf_log("after: %p->%p\n", dest, *dest);
            #endif // DYN_DEBUG_LOGGING
        } else {
            #ifdef DYN_DEBUG_LOGGING
            printf_log("Symbol not provided in specified file!\n");
            #endif // DYN_DEBUG_LOGGING
        }
    }
}

void attempt_relocations(const struct dyn_loaded* dyn_main, const struct dyn_loaded* dyn_provider) {
    // got
    if (dyn_main->rela != NULL) {
        #ifdef DYN_DEBUG_LOGGING
        printf_log("relocating main GOT entries\n");
        #endif // DYN_DEBUG_LOGGING
        relocate_single_rela(dyn_main, dyn_provider, dyn_main->rela, dyn_main->rela_cnt);
    }
    // plt
    if (dyn_main->relaplt != NULL) {
        #ifdef DYN_DEBUG_LOGGING
        printf_log("relocating PLT entries\n");
        #endif // DYN_DEBUG_LOGGING
        relocate_single_rela(dyn_main, dyn_provider, dyn_main->relaplt, dyn_main->relaplt_cnt);
    }
}

void add_tls_module(const struct dyn_loaded* dyn, void** cluster_stack) {
    if (dyn->tls_phdr == NULL) {
        printf_log("Error: This file does not have a TLS section!\n");
        return;
    }
    uint32_t* old_dtv = (uint32_t *)*cluster_stack;
    uint32_t dtv_size = *old_dtv;
    Elf32_Phdr* phdr = dyn->tls_phdr;
    size_t new_space = phdr->p_memsz;
    if (dtv_size < dyn->tls_module_id) {
        // need to extend the dtv to fit more modules
        new_space += (dyn->tls_module_id - dtv_size) * sizeof(uint32_t);
        // increase dtv size
        *old_dtv = dyn->tls_module_id;
    }
    uint32_t* new_dtv = *cluster_stack - new_space;
    // move the dtv to the top
    #ifdef DYN_DEBUG_LOGGING
    printf_log("TLS: dtv size before %lu, after %lu, added space: %lx\n", dtv_size, *old_dtv, new_space);
    printf_log("TLS: moving the dtv - copying %x bytes from %p to %p\n", dtv_size + 1, old_dtv, new_dtv);
    #endif // DYN_DEBUG_LOGGING
    memcpy(new_dtv, *cluster_stack, (dtv_size + 1) * sizeof(uint32_t));
    void* tls_start = new_dtv + *new_dtv + 1;

    // add the new code to the gap
    #ifdef DYN_DEBUG_LOGGING
    printf_log("TLS: moving the tls section - copying %x bytes from %p to %p\n", phdr->p_filesz, dyn->load_start + phdr->p_vaddr, tls_start);
    #endif // DYN_DEBUG_LOGGING
    memcpy(tls_start, dyn->load_start + phdr->p_vaddr, phdr->p_filesz);

    // make the new dtv entry
    new_dtv[dyn->tls_module_id] = (uintptr_t)tls_start;
    // update the stack pointer
    *cluster_stack = new_dtv;
}
