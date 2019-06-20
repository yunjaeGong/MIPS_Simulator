#ifndef _WEEK10_REGISTERS_MIPS_MEMORY_H
#define _WEEK10_REGISTERS_MIPS_MEMORY_H

#define READ 0
#define WRITE 1
#define REG_MAX 32
#define PC_INIT 0x400000
#define SP_INIT 0x80000000

enum REGS {
    $zero,
    $at, // designated to assembler only
    $v0,$v1,
    $a0,$a1,$a2,$a3,
    $t0,$t1,$t2,$t3,$t4,$t5,$t6,$t7,
    $s0,$s1,$s2,$s3,$s4,$s5,$s6,$s7,
    $t8,$t9,
    $k0,$k1,
    $gp,$sp,$fp,$ra
};

extern unsigned int PC, HI, LO;
extern unsigned int R[32];

unsigned int REG(unsigned int A, unsigned int V, unsigned int nRW);
const char* getREGname(enum REGS regs);
unsigned int getRegister(unsigned int A);
unsigned setRegister(unsigned A, unsigned V);
void setPC(unsigned int val);
unsigned int getPC_Offset(void);

#endif //_WEEK10_REGISTERS_MIPS_MEMORY_H
