# Documentation for the dynamic linker

# Getting Started
The easiest way is to use the VS Code devcontainer configuration.
1. Clone the repository including recursive submodules (`git clone --recursive ...`)
2. Open in VS Code. When prompted choose "reopen in container".
3. The devcontainer will be built. This can take a while
4. Wait for the clangd and CMake extensions to be installed. This will also take a moment without progress being shown.
    Once everything has been installed a CMake entry should appear in the sidebar.
5. Ensure that the "Chimera (Open)" CMake preset is set. This can be changed through the CMake sidebar menu under
    "Configure".
6. Click the "Build" button at the bottom left of the window. This should build all tests.


# Linker Implementation
The main implementation of the linker is located at `host/runtime/src/dynamic.c`. Metadata about loaded objects is
stored in a `struct dyn_loaded`.

For concrete examples on usage of the linker, the tests should also be consulted.

## Loading shared objects
Loading of shared objects into memory is the first step of a dynamic linking process. This happens through the
`load_so(struct dyn_loaded* dyn, unsigned char* so_start)` function. `dyn` is a pointer to an empty `struct dyn_loaded`
which will be populated with metadata and `so_start` points to the start of an ELF file in memory.

This function will
1. Verify the file is an ELF file for RISC-V
1. Determine the required space for loading by looking at the `LOAD` sections in the program header.
1. Allocate the required space on the memory island
1. Process the program headers
    - `LOAD`: Load the specified segment into the allocated memory
    - `DYNAMIC`: Store a pointer to the dynamic section
    - `TLS`: Store a pointer to the TLS program header and assign a module ID to the shared object.
1. If a dynamic section was found, additionally read it and store various useful information discovered
    from it.
1. Return how much data was loaded.

## Performing relocations
Two functions are provided to perform relocations. `attempt_relocations(const struct dyn_loaded* dyn_main, const struct dyn_loaded* dyn_provider)`. Will relocate all symbols required in `dyn_main` provided by `dyn_provider`.
This is the main way relocations will usually be performed.

This function will, for the GOT and then the PLT loop through all provided relocations, attempt to resolve the
symbol in the providing file and perform the relocation.

Additionally, there is also the function `relocate_single_symbol(const struct dyn_loaded* dyn_main, bool useplt, const char* symtarget, void* value)`.
This function allows manually performing a relocation. The symbol with the name `symtarget` will be relocated with
value `value`. `useplt` specifies whether the relocation should be searched for in the PLT (if true) or GOT (if false).

## Retrieving a symbol value
The function `get_symbol_pointer(const struct dyn_loaded* dyns, const char* name)` returns the value of a single
symbol with `name` from the shared object loaded at `dyns`.

## TLS
TLS is complicated and therefore the implementation is not fully functional (more on this in the limitations section).
Each shared object having a TLS section is assigned a module ID when being loaded. Then, when a TLS symbol is being
relocated, two relocations take place, one writing the module ID and the other writing the offset to the symbol within
the module.

Then, before executing code from a shared object which requires TLS, the function `add_tls_module(const struct dyn_loaded* dyn, void** cluster_stack)`.
`dyn` should be the loaded module providing the TLS section. `cluster_stack` should be a pointer to a pointer to the
stack for the corresponding execution context. This function will extend the stack and add the TLS section to it. In
order to find the right TLS section for the right module if multiple are loaded, at the beginning of the stack there
is the so called dynamic thread vector (DTV) which will also be updated by this function. During runtime, the thread
pointer points to the start of the DTV. The first word in the DTV is simply a value signifying its own length. This is
followed by a table of offsets for all TLS modules. So given a module ID and offset a lookup into the TLS section for
a thread would be done as `tp[id] + offset`.

*For this to work correctly, the DTV needs to be initialised with a single entry reading 0 before calling `add_tls_module`!* See the "tls" test.

# CMake
A custom CMake function `add_cluster_shared_target` was added to `tests/CMakeLists.txt` to help with compiling
shared objects and embedding them into the data section of a binary. It is quite hacky.
To use it, within the `CMakeLists.txt` file for a single test:

1. Create a custom target to represent all wrapped binaries: `add_custom_target(${TEST_NAME}_binwrap)`. This has to
    have exactly that name.
1. For each shared object, create a target for it with `add_cluster_shared_target(name)`. Here the name can be anything.
1. Add source files to the target with `target_sources(name PUBLIC src/program.c)`. Other operations can also be done,
    treating `name` as a normal CMake target.
1. When compiling the main test, link the wrapped binary into it:
    `target_link_libraries(${TEST_NAME} PUBLIC "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libname_wrapped.o")` whereby `name`
    is substituted with the previously chosen name.
1. Finally, add a manual dependency between the main binary and all the shared objects.
    `add_dependencies(${TEST_NAME} ${TEST_NAME}_binwrap)`

All shared objects and their wrapped versions are built into `build/lib/` and have a name starting with `lib`.


# Tests
A few tests were added to test and evaluate the dynamic linker. All of them are located under `tests/snitchCluster/`

## dynamic
The simplest example for usage of the dynamic linker, consisting of a main cluster componentn which provides two
functions: One retrieves the value of `libraryVariant` and one forwards the function call to `clusterWork`.
These two symbols will be provided through a second work module. Which module is loaded is controlled by the call
to `load_so` in `test_host.c`.

## dynlibc
A little more sophisticated example which uses the cluster to make a string lowercase. The following components exist
- `cluster_static.c`: Statically linked cluster code. This just contains the cluster interrupt handler.
- `test_cluster.c`: Entry point, same as for "dynamic" example providing library variant and work function wrapper functions.
- `cluster_work1.c`: Variant 1 of the work function, transforms a string to lowercase using `tolower()`.
- `cluster_work2.c`: Variant 2 of the work function, additionally reverses the string.
- `tolower.c`: Taken from libc, function to make a single character lowercase.

Three versions of this example are generated, the main host code at `test_host.c` changes its behavior based on which
preprocessor definitions are set.
- `VARIANT_STATIC`: Fully statically linked
- `VARIANT_MIXED`: Mixed version, cluster code is linked dynamically but itself linked statically to libc.
- `VARIANT_DYN`: Cluster code is fully dynamic, the `tolower()` function is provided as a separate shared object.

## dynploy
This uses code from Deeploy in order to perform the calculation of a single stage from a neural network. The general
structure and principle is similar as for the "dynlibc" example but with more involved code, most of which has been
copied from Deeploy. An additional change is that the different variants are not implemented through differing
preprocessor definitions for the main `test_host.c` but instead through inclusion of either `func_dyn.c` or
`func_static.c` for the static host code. The difference between the two variants here is that one uses a standard
Gemm kernel while the other one uses a "redundant" version which is supposed to perform the calculations twice.
They are both located unter `.../dynploy/src_cluster/deeploysnitchlib/src/Gemm_s8...`.

The redundant version actually does not seem to work correctly, likely due to compiler optimizations.

**Warning: Due to space limitations, debug symbols have to be disabled to run the dynamic version of this test**.
To disable these, comment out line 58 of `cmake/toolchain_llvm.cmake`.

## tls
Demonstration of dynamic linking a module containing a TLS section.

# Limitations and further work
Being a minimal implementation, the linker has many limitations:
- Only one `RELA` relocation table is supported per shared object. The specification allows having multiple.
    `REL` tables are not supported at all (These do not contain addends and are smaller).
- Only the following relocation types are supported
    - `RISCV_RELATIVE`: only when an object is linked to itself
    - `RISCV_32`: "Standard" relocation
    - `RISCV_JUMP_SLOT`: Relocation of a function (= PLT entry)
    - `RISCV_TLS_DTPMOD32` and `RISCV_TLS_DTPREL32`: Relocation of TLS variable
- TLS is only supported for shared objects accessing their own TLS section. Sharing a TLS variable across different
    shared objects leads to some problems, but I do not remember what those problems were.

## Approach to dealing with many libraries
Currently, all relocations happen in a one-to-one fashion between loaded objects. This has some issues
- For a complex application with many cross-dependencies, linking becomes inefficient and error-prone, as each component
    needs to be linked manually
- There is no reliable way to determine whether a relocation has already been performed or not.

A better approach would be to add a collection of shared objects to a process image and then provide a mechanism which
performs relocation from and to all components at once. This would allow a more efficient implementation as each
relocation table only needs to be traversed once. Also, not finding a symbol could then actually be considered an error
instead of having to assume that it might still be linked later.


# External files
Some external files are included within this repo which should be kept in mind for licensing reasons
- `host/runtime/inc/elf.h`: This provides the values and structures for reading an ELF file. Taken from the GNU C
    library and licensed under LGPL. Should be redistributable.
- `tests/snitchCluster/dynlibc/src_cluster/tolower.c`: Taken from picolibc. Licensed under BSD-3. Should also be
    redistributable. The only reason it is included is because I did not try to automatically fetch it from the
    picolibc repo through some CMake magic.

