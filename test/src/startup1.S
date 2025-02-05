### NIIET K1921VG015 startup code
###
### @copyright (C) NIIET 2022. All rights reserved.

#include "arch.h"
#include "memasm.h"

### #########################
### startup code

    .globl _start, main, exit, abort, trap_handler, trap_entry, _hart_halt, plf_init, app_init
    .weak exit, abort, trap_entry, trap_handler, app_init

    .option norvc

    .section ".startup.entry","ax",@progbits

    ## Entry point
_start:
    ## reset mstatus
    csrw  mstatus, zero

    ## setup gp
    la gp, __global_pointer$
    ## setup MTVEC
    la t0, trap_entry
    csrw  mtvec, t0
    ## setup MIE, MIP
    
    csrw  mie, zero
    csrw  mip, zero

    ## init FPU
    li    a0, (1 << 13) // mstatus.FS = 1 (initial)
    csrs  mstatus, a0
    csrw  fcsr, zero

    ## init HART#0 sp, tp
    la sp, __STACK_END__
    mv    tp, sp
    csrw  mscratch, sp

    
    ## platform init
    
    #my_comment la t0, plf_init
    #my_comment jalr  t0
    #my_comment: init bss, data, sdata

#bss
    la t0, __bss_start
    la t1, __bss_end
1:
	bgt t0, t1, 2f
	sw zero, 0(t0)
    addi t0, t0, 4
    j 1b
2:
#data
	la t0, __data_target_start
	la t1, __data_target_end
	la t2, __data_source_start
1:
	bgt t0, t1, 2f
	lw t3, 0(t2)
    sw t3, 0(t0)
    addi t0, t0, 4
    addi t2, t2, 4
    j 1b
2:
#sdata
	la t0, __sdata_target_start
	la t1, __sdata_target_end
	la t2, __sdata_source_start
1:
	bgt t0, t1, 2f
	lw t3, 0(t2)
    sw t3, 0(t0)
    addi t0, t0, 4
    addi t2, t2, 4
    j 1b
2:

    #my_comment_end
    
    ## jal  plf_init
    ## application-specific initialization hook
    la t0, app_init
    jalr  t0
    ## jal  app_init
    ## main() args
    li    a0, 0
    li    a1, 0
    la t0, main
    la ra, exit
    jr    t0

trap_handler:
_hart_halt:
    ## disable interrupts
    csrci mstatus, 0xf
    csrw  mie, zero
1:  wfi
    j     1b

exit:
    nop
abort:
    j     _hart_halt


app_init:
    ret

### #########################
### trap handler
    .section ".text.crt.trap_entry","ax",@progbits
    .align 6
    .type trap_entry, @function
trap_entry:

###
j trap_handler
###
/*
    ## save context
    context_save
    ## save mstatus priv stack
    csrr s0, mstatus
    ## load trap handler args
    csrr a0, mcause
    csrr a1, mepc
    mv   a2, sp

    ## setup gp
    la gp, __global_pointer$
    ## call trap handler
    la t0, trap_handler
    jalr t0

    ## restore mstatus priv stack
    csrw mstatus, s0
    ## restore context
    context_restore
    mret
    .size trap_entry, .-trap_entry
*/