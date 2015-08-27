# 1 "../common/nf2.S"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../common/nf2.S"
# 13 "../common/nf2.S"
# 1 "../common/num_cpu.h" 1
# 14 "../common/nf2.S" 2

.equ CONNADDR, (1<<26)


.equ _spdiff, 32 # power of 2, adds to stack diff in init_stack
.text
.align 2
.globl _start
.globl _t1
.globl _exit
.ent _start
_start:
.set noreorder

    nop # Must start with nop since 1st instr executed twice

    nop
    nop

    lui $28,%hi(_gp) # set the global data pointer (value in lsi.ld)
    addiu $28,$28,%lo(_gp)
    lui $29,%hi(_sp) # set the stack pointer (value in lsi.ld)
    addiu $29,$29,%lo(_sp)

    addiu $26,$0,0
    jal init_stack
    jal main
    nop
    j _exit

.end _start


.globl _t1
.ent _t1
_t1: # this is for ST
_t01:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-1*_spdiff)
 addiu $26,$0,1
 jal init_stack
 jal main
 j _exit
 nop
.end _t1
.globl _t2
.ent _t2
_t2: # this is for ST
_t02:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-2*_spdiff)
 addiu $26,$0,2
 jal init_stack
 jal main
 j _exit
 nop
.end _t2
.globl _t3
.ent _t3
_t3: # this is for ST
_t03:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-3*_spdiff)
 addiu $26,$0,3
 jal init_stack
 jal main
 j _exit
 nop
.end _t3
.globl _t4
.ent _t4
_t4: # this is for ST
_t04:
 nop
 nop



 nop

 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-4*_spdiff)
 addiu $26,$0,4
 jal init_stack
 jal main
 j _exit
 nop
.end _t4
.globl _t5
.ent _t5
_t5: # this is for ST
_t05:
 nop
 nop



 nop

 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-5*_spdiff)
 addiu $26,$0,5
 jal init_stack
 jal main
 j _exit
 nop
.end _t5
.globl _t6
.ent _t6
_t6: # this is for ST
_t06:
 nop
 nop



 nop

 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-6*_spdiff)
 addiu $26,$0,6
 jal init_stack
 jal main
 j _exit
 nop
.end _t6
.globl _t7
.ent _t7
_t7: # this is for ST
_t07:
 nop
 nop



 nop

 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-7*_spdiff)
 addiu $26,$0,7
 jal init_stack
 jal main
 j _exit
 nop
.end _t7
.globl _t8
.ent _t8
_t8: # this is for ST
_t08:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-8*_spdiff)
 addiu $26,$0,8
 jal init_stack
 jal main
 j _exit
 nop
.end _t8
.globl _t9
.ent _t9
_t9: # this is for ST
_t09:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-9*_spdiff)
 addiu $26,$0,9
 jal init_stack
 jal main
 j _exit
 nop
.end _t9




.globl _t10
.ent _t10
_t10: # this is for ST
_t010:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-10*_spdiff)
 addiu $26,$0,10
 jal init_stack
 jal main
 j _exit
 nop
.end _t10
.globl _t11
.ent _t11
_t11: # this is for ST
_t011:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-11*_spdiff)
 addiu $26,$0,11
 jal init_stack
 jal main
 j _exit
 nop
.end _t11
.globl _t12
.ent _t12
_t12: # this is for ST
_t012:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-12*_spdiff)
 addiu $26,$0,12
 jal init_stack
 jal main
 j _exit
 nop
.end _t12
.globl _t13
.ent _t13
_t13: # this is for ST
_t013:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-13*_spdiff)
 addiu $26,$0,13
 jal init_stack
 jal main
 j _exit
 nop
.end _t13
.globl _t14
.ent _t14
_t14: # this is for ST
_t014:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-14*_spdiff)
 addiu $26,$0,14
 jal init_stack
 jal main
 j _exit
 nop
.end _t14
.globl _t15
.ent _t15
_t15: # this is for ST
_t015:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-15*_spdiff)
 addiu $26,$0,15
 jal init_stack
 jal main
 j _exit
 nop
.end _t15
.globl _t16
.ent _t16
_t16: # this is for ST
_t016:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-16*_spdiff)
 addiu $26,$0,16
 jal init_stack
 jal main
 j _exit
 nop
.end _t16
.globl _t17
.ent _t17
_t17: # this is for ST
_t017:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-17*_spdiff)
 addiu $26,$0,17
 jal init_stack
 jal main
 j _exit
 nop
.end _t17
.globl _t18
.ent _t18
_t18: # this is for ST
_t018:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-18*_spdiff)
 addiu $26,$0,18
 jal init_stack
 jal main
 j _exit
 nop
.end _t18
.globl _t19
.ent _t19
_t19: # this is for ST
_t019:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-19*_spdiff)
 addiu $26,$0,19
 jal init_stack
 jal main
 j _exit
 nop
.end _t19
.globl _t20
.ent _t20
_t20: # this is for ST
_t020:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-20*_spdiff)
 addiu $26,$0,20
 jal init_stack
 jal main
 j _exit
 nop
.end _t20
.globl _t21
.ent _t21
_t21: # this is for ST
_t021:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-21*_spdiff)
 addiu $26,$0,21
 jal init_stack
 jal main
 j _exit
 nop
.end _t21
.globl _t22
.ent _t22
_t22: # this is for ST
_t022:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-22*_spdiff)
 addiu $26,$0,22
 jal init_stack
 jal main
 j _exit
 nop
.end _t22
.globl _t23
.ent _t23
_t23: # this is for ST
_t023:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-23*_spdiff)
 addiu $26,$0,23
 jal init_stack
 jal main
 j _exit
 nop
.end _t23
.globl _t24
.ent _t24
_t24: # this is for ST
_t024:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-24*_spdiff)
 addiu $26,$0,24
 jal init_stack
 jal main
 j _exit
 nop
.end _t24
.globl _t25
.ent _t25
_t25: # this is for ST
_t025:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-25*_spdiff)
 addiu $26,$0,25
 jal init_stack
 jal main
 j _exit
 nop
.end _t25
.globl _t26
.ent _t26
_t26: # this is for ST
_t026:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-26*_spdiff)
 addiu $26,$0,26
 jal init_stack
 jal main
 j _exit
 nop
.end _t26
.globl _t27
.ent _t27
_t27: # this is for ST
_t027:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-27*_spdiff)
 addiu $26,$0,27
 jal init_stack
 jal main
 j _exit
 nop
.end _t27
.globl _t28
.ent _t28
_t28: # this is for ST
_t028:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-28*_spdiff)
 addiu $26,$0,28
 jal init_stack
 jal main
 j _exit
 nop
.end _t28
.globl _t29
.ent _t29
_t29: # this is for ST
_t029:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-29*_spdiff)
 addiu $26,$0,29
 jal init_stack
 jal main
 j _exit
 nop
.end _t29
.globl _t30
.ent _t30
_t30: # this is for ST
_t030:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-30*_spdiff)
 addiu $26,$0,30
 jal init_stack
 jal main
 j _exit
 nop
.end _t30
.globl _t31
.ent _t31
_t31: # this is for ST
_t031:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-31*_spdiff)
 addiu $26,$0,31
 jal init_stack
 jal main
 j _exit
 nop
.end _t31
.globl _t32
.ent _t32
_t32: # this is for ST
_t032:
 nop
 nop
 nop
 lui $28,%hi(_gp)
 lui $29,%hi(_sp)
 addiu $28,$28,%lo(_gp)
 addiu $29,$29,%lo(_sp-32*_spdiff)
 addiu $26,$0,32
 jal init_stack
 jal main
 j _exit
 nop
.end _t32






.ent _exit
_exit:
    lui $28, %hi(_gp)
    ori $28, $28, %lo(_gp)
    lui $2, 0xdead
    ori $2, $2, 0xdead
    sw $2, 0($28) # write 0xdeaddead overwriting results (who cares)
    lui $2, 0
    ori $16, $0, 0
    ori $17, $0, 0
    ori $18, $0, 0
    ori $19, $0, 0
    ori $20, $0, 0
    ori $21, $0, 0
    ori $22, $0, 0
    ori $23, $0, 0
    ori $30, $0, 0
    j _exit
    nop
    nop
    nop

    .set reorder
    .end _exit
