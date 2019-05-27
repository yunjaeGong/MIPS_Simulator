#ifndef WEEK11_MIPS_SIM_MIPS_ALU_H
#define WEEK11_MIPS_SIM_MIPS_ALU_H

// ALU Control

#define ALU_CTRL_SLL 1
#define ALU_CTRL_SRL 2
#define ALU_CTRL_SRA 3
#define ALU_CTRL_SLT 4
#define ALU_CTRL_ADD 8
#define ALU_CTRL_SUB 9
#define ALU_CTRL_AND 12
#define ALU_CTRL_OR 13
#define ALU_CTRL_XOR 14
#define ALU_CTRL_NOR 15

int ALU(int X, unsigned Y, int C, int *Z, char** op);

#endif //WEEK11_MIPS_SIM_MIPS_ALU_H
