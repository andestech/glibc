#include "sys/asm.h"
#define UFLOG 3
#define UF (1 << UFLOG)

  .p2align  2
  .global   memcpy
  .type     memcpy, @function
memcpy:
  beqz  a2, .Lret
  mv    t0, a0
  mv    t2, a2
.Lmultiword_mode:
  andi  t1, t2, ~(SZREG * UF - 1)
  add   t1, t0, t1
  bge   a0, t1, .Lword_mode
  .p2align  2
.Lmultiword_loop:
  REG_L a2, SZREG*0(a1)
  REG_L a3, SZREG*1(a1)
  REG_L a4, SZREG*2(a1)
  REG_L a5, SZREG*3(a1)
  REG_S a2, SZREG*0(a0)
  REG_S a3, SZREG*1(a0)
  REG_S a4, SZREG*2(a0)
  REG_S a5, SZREG*3(a0)
  REG_L a2, SZREG*4(a1)
  REG_L a3, SZREG*5(a1)
  REG_L a4, SZREG*6(a1)
  REG_L a5, SZREG*7(a1)
  REG_S a2, SZREG*4(a0)
  REG_S a3, SZREG*5(a0)
  REG_S a4, SZREG*6(a0)
  REG_S a5, SZREG*7(a0)
  addi  a1, a1, SZREG * UF
  addi  a0, a0, SZREG * UF
  blt   a0, t1, .Lmultiword_loop
.Lword_mode:
  andi  t1, t2, ~(SZREG - 1)
  add   t1, t0, t1
  bge   a0, t1, .Lbyte_mode
  .p2align  2
.Lword_loop:
  REG_L a2, 0(a1)
  addi  a1, a1, SZREG
  REG_S a2, 0(a0)
  addi  a0, a0, SZREG
  blt   a0, t1, .Lword_loop
.Lbyte_mode:
  add   t1, t0, t2
  bge   a0, t1, .Lend
  .p2align  2
.Lbyte_loop:
  lbu   a2, 0(a1)
  addi  a1, a1, 1
  sb    a2, 0(a0)
  addi  a0, a0, 1
  blt   a0, t1, .Lbyte_loop
.Lend:
  mv    a0, t0
.Lret:
  ret
  .size     memcpy, . - memcpy

libc_hidden_builtin_def (memcpy)
