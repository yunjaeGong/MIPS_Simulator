#ifndef WEEK4_DISASSEMBLE_MIPS_HEADER_H
#define WEEK4_DISASSEMBLE_MIPS_HEADER_H

#define OP 0xFC000000
#define RS 0x03E00000
#define RT 0x001F0000
#define RD 0x0000F800
#define SH 0x000007C0
#define FN 0x0000003F
#define OPERAND_OFFSET 0x0000FFFF
#define ADDRESS 0x03FFFFFF

#define UPPER 0b111000
#define LOWER 0b000111

// R Type
// UPPER
#define SHIFT 0
#define JUMPR_SYSCALL 1
#define MOVE 2
#define MUL 3
#define R_ARITH_LOGIC 4
#define SET_LT 5

// LOWER
#define ADD 0
#define SUB 2
#define AND 4
#define OR 5
#define XOR 6
#define NOR 7
#define SLT 2
#define JR 0
#define SYSCALL 4
#define MFHI 0
#define MFLO 2


// I Type
// UPPER
#define JUMP_OR_BRANCH 0
#define I_ARITH_LOGIC 1
#define LOAD 4
#define STORE 5

// LOWER
#define SYSCALL 4
#define BLTZ 1
#define JUMP 2
#define JAL 3
#define BEQ 4
#define BNE 5
#define SRL 2
#define SLL 0
#define SRA 3
#define ADDI 0
#define SLTI 2
#define ANDI 4
#define ORI 5
#define XORI 6
#define LUI 7

extern char* CPU_REG[32]; // list of cpu register

static union Instruction {
    unsigned int INST;
    struct R_type {
        unsigned int fn: 6;
        unsigned int sh: 5;
        unsigned int rd: 5;
        unsigned int rt: 5;
        unsigned int rs: 5;
        unsigned int op: 6;
    } R;
    struct I_type { // for I type Instructions
        int immi: 16;
        unsigned int rt: 5,rs: 5, op: 6;
    } I;
    struct J_type {
        int adr: 26;
        unsigned int op: 6;
    } J;
}IR;

int total_bytes(FILE* const fp);
unsigned number_of_Inst (FILE* const fp);
unsigned number_of_Data (FILE* const fp);
unsigned get_Instruction(unsigned adr, int offset);

#endif //WEEK4_DISASSEMBLE_MIPS_HEADER_H
