# x86_desc.S - Set up x86 segment descriptors, descriptor tables
# vim:ts=4 noexpandtab

#define ASM     1
#include "x86_desc.h"

.text

.globl ldt_size, tss_size
.globl gdt_desc, ldt_desc, tss_desc
.globl tss, tss_desc_ptr, ldt, ldt_desc_ptr
.globl gdt_ptr
.globl idt_desc_ptr, idt

.align 4


tss_size:
    .long tss_bottom - tss - 1

ldt_size:
    .long ldt_bottom - ldt - 1

    .word 0 # Padding
ldt_desc:
    .word KERNEL_LDT
    .long ldt

    .align 4

tss:
_tss:
    .rept 104
    .byte 0
    .endr
tss_bottom:

    .align  16
gdt:
_gdt:

    # First GDT entry cannot be used
    .quad 0

    # NULL entry
    .quad 0

    # Segmentation will not be used
    # CS and DS both are 0-4GB r/w segments
    #
    # The layout is (from Intel IA-32 reference manual):
    #  31        24 23  22  21  20  19   16 15  14 13 12  11   8 7          0
    # |----------------------------------------------------------------------|
    # |            |   | D |   | A |  Seg  |   |  D  |   |      |            |
    # | Base 31:24 | G | / | 0 | V | Limit | P |  P  | S | Type | Base 23:16 |
    # |            |   | B |   | L | 19:16 |   |  L  |   |      |            |
    # |----------------------------------------------------------------------|
    #
    # |----------------------------------------------------------------------|
    # |                                    |                                 |
    # | Base 15:0                          | Segment Limit 15:0              |
    # |                                    |                                 |
    # |----------------------------------------------------------------------|

gdt_ptr:
    # Set up an entry for kernel CS
    .quad 0x00CF9A000000FFFF

    # Set up an entry for kernel DS
    .quad 0x00CF92000000FFFF

    # Set up an entry for user CS
    .quad 0x00CFFA000000FFFF

    # Set up an entry for user DS
    .quad 0x00CFF2000000FFFF

    # Set up an entry for TSS
tss_desc_ptr:
    .quad 0

    # Set up one LDT
ldt_desc_ptr:
    .quad 0

gdt_bottom:

    .align 16
ldt:
    .rept 4
    .quad 0
    .endr
ldt_bottom:

.align 4
    .word 0 # Padding

# Replicate desc pointer set up from idt since we use same format for loading gdt in lgdt as we do with idt in lidt
gdt_desc:
    .word gdt_bottom - gdt - 1
    .long gdt

idt_desc_ptr:
    .word idt_bottom - idt - 1
    .long idt


    .align  16
idt:
_idt:
    .rept NUM_VEC
    .quad 0
    .endr

idt_bottom:


# redundant exception wrapper we dont care about this.

.globl diverror_asm, debug_asm, nmi_asm, breakpoint_asm, overflow_asm, boundrange_asm, invalidop_asm, device_na_asm, doublefault_asm
.globl invalidtss_asm, segment_na_asm, stack_seg_asm, general_protection_asm, page_fault_asm, floating_excep_asm, alignment_check_asm 
.globl machine_check_asm, smd_asm, coprocessor_asm

diverror_asm: 
pushal
pushfl
call diverror
popal
popfl
iret

debug_asm:
pushal
pushfl
call debug
popal
popfl
iret

nmi_asm:
pushal
pushfl
call nmi
popal
popfl
iret

breakpoint_asm:
pushal
pushfl
call breakpoint
popal
popfl
iret

overflow_asm:
pushal
pushfl
call overflow
popal
popfl
iret

boundrange_asm:
pushal
pushfl
call boundrange
popal
popfl
iret

invalidop_asm:
pushal
pushfl
call invalidop
popal
popfl
iret

device_na_asm:
pushal
pushfl
call device_na
popal
popfl
iret

doublefault_asm:
pushal
pushfl
call doublefault
popal
popfl
iret

invalidtss_asm:
pushal
pushfl
call invalidtss
popal
popfl
iret

segment_na_asm:
pushal
pushfl
call segment_na
popal
popfl
iret

stack_seg_asm:
pushal
pushfl
call stack_seg
popal
popfl
iret

general_protection_asm:
cli
pushal
pushfl
call general_protection
popal
popfl
sti
iret

page_fault_asm:
cli
pushal
pushfl
call page_fault
popal
popfl
sti
iret

floating_excep_asm:
pushal
pushfl
call floating_excep
popal
popfl
iret

alignment_check_asm:
pushal
pushfl
call alignment_check
popal
popfl
iret

machine_check_asm:
pushal
pushfl
call machine_check
popal
popfl
iret

smd_asm:
pushal
pushfl
call smd
popal
popfl
iret

coprocessor_asm:
pushal
pushfl
call coprocessor
popal
popfl
iret

