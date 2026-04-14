/* SPDX-FileCopyrightText: 2022 ETH Zurich and University of Bologna */
/* SPDX-License-Identifier: Apache-2.0 */

/* This header defines symbols and rules universal to bare-metal execution */

SECTIONS {
  /* Keep binaries lean */
  /DISCARD/ : { *(.riscv.attributes) *(.comment) }

  /* Global and stack pointer */
  /* By default, keep the calling context (boot ROM) stack pointer */
  /*
  __global_pointer$ = ADDR(.misc) + SIZEOF(.misc) / 2;
  __stack_pointer$  = __stack_start - 8;
  */

  __base_l1_alias = 0x18000000;

  /* Further addresses */
  __base_dma      = 0x01000000;
  __base_bootrom  = 0x02000000;
  __base_clint    = 0x02040000;
  __base_axirt    = 0x020C0000;
  __base_axirtgrd = 0x020C1ffc;
  __base_regs     = 0x03000000;
  __base_llc      = 0x03001000;
  __base_uart     = 0x03002000;
  __base_i2c      = 0x03003000;
  __base_spih     = 0x03004000;
  __base_gpio     = 0x03005000;
  __base_slink    = 0x03006000;
  __base_vga      = 0x03007000;
  __base_usb      = 0x03008000;
  __base_bus_err  = 0x03009000;
  __base_plic     = 0x04000000;
  __base_clic     = 0x08000000;
  __base_dram     = 0x80000000;
  __base_memisl   = 0x48000000;
  /* __stack_start   = ORIGIN(memisl) + LENGTH(memisl); */
}
