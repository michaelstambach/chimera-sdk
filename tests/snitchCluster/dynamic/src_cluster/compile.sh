#!/bin/bash

# this is just a list of commands before i get this working in CMake

# compile files as position independent
/app/install/llvm/bin/clang --target=riscv32-unknown-elf -I /workspaces/chimera-sdk/build/picolibc-install/include -I /workspaces/chimera-sdk/targets/chimera-open/inc -I /workspaces/chimera-sdk/devices/snitch_cluster -I /workspaces/chimera-sdk/host/drivers -I ../include \
-ggdb -gdwarf-4 -gstrict-dwarf -O2 -march=rv32imafd_xdma -mabi=ilp32 -std=gnu11 -c test_cluster.c cluster_ret{1,2}.c -fPIC -mno-relax -shared

# link them to shared objects
/app/install/llvm/bin/clang --target=riscv32-unknown-elf -L /workspaces/chimera-sdk/build/picolibc-install/lib/rv32im/ilp32 -L /app/install/llvm/lib/clang/15.0.0/lib/baremetal/rv32im/ -T link.ld -Xlinker -shared -o test_cluster.so test_cluster.o
/app/install/llvm/bin/clang --target=riscv32-unknown-elf -L /workspaces/chimera-sdk/build/picolibc-install/lib/rv32im/ilp32 -L /app/install/llvm/lib/clang/15.0.0/lib/baremetal/rv32im/ -Xlinker -shared -o cluster_ret1.so cluster_ret1.o
/app/install/llvm/bin/clang --target=riscv32-unknown-elf -L /workspaces/chimera-sdk/build/picolibc-install/lib/rv32im/ilp32 -L /app/install/llvm/lib/clang/15.0.0/lib/baremetal/rv32im/ -Xlinker -shared -o cluster_ret2.so cluster_ret2.o

# embed main library as "file" into data section
/app/install/llvm/bin/llvm-objcopy -I binary -O elf32-littleriscv test_cluster.so test_cluster_so_bin.o
/app/install/llvm/bin/llvm-objcopy -I binary -O elf32-littleriscv cluster_ret1.so cluster_ret1_so_bin.o
/app/install/llvm/bin/llvm-objcopy -I binary -O elf32-littleriscv cluster_ret2.so cluster_ret2_so_bin.o
