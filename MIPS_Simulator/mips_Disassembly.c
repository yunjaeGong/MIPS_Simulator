#include<stdio.h>
#include <string.h>
#include <stdint.h>
#include "mips_header.h"
#include "mips_Disassembly.h"
#include "mips_Memory.h"
#include "mips_ALU.h"
#include "mips_Register.h"

char* CPU_REG[32] = {
        "$a0","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0",
        "$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"
};


int total_bytes(FILE* const fp) { // total number of bytes in a file
    fseek(fp,0,SEEK_END);
    int n = ftell(fp);
    fseek(fp,0,SEEK_SET);
    return n;
}
// 함수에서도 호출 가능하게 overloading -> fp대신 메모리
unsigned number_of_Inst (FILE* const fp) { // 명령어 개수 반환
    unsigned num;
    fseek(fp,0,SEEK_SET);
    fread(&num, sizeof(unsigned int),1,fp);
    fseek(fp,0,SEEK_SET);
    num = endian_swap(num,WORD);
    return num;
}

unsigned number_of_Data (FILE* const fp) { // 데이터 개수 반환
    unsigned num;
    fseek(fp,4,SEEK_SET);
    fread(&num, sizeof(unsigned int),1,fp);
    fseek(fp,0,SEEK_SET);
    num = endian_swap(num,WORD);
    return num;
}

unsigned get_Instruction(unsigned adr, int offset) {
    /* adr: Base address of memory to read
     * offset: from where to read
     * O, F: opcode, funct 를 main으로 전달하기 위한 변수*/

    //IR.INST = endian_swap(MEM(adr,0,READ,WORD),WORD);
    IR.INST = getMemory(adr,WORD); // Fetch an Instruction from memory address adr
    char* name = NULL;
    unsigned ret;
    int size;
    // ALU
    int X, C, Z;
    // Flow control
    unsigned nPC;
    unsigned Y;
    // Load / Store
    unsigned nAdr;

    unsigned   IR_R_FN_LOWER = IR.R.fn & (unsigned)LOWER,
            IR_R_FN_UPPER = (IR.R.fn & (unsigned)UPPER)>>(unsigned)3,
            IR_I_OP_LOWER = IR.I.op & (unsigned)LOWER,
            IR_I_OP_UPPER = (IR.I.op & (unsigned)UPPER)>>(unsigned)3;

    // Instruction names
    if(IR.R.op == 0) { // R_Type Instructions
        switch(IR.R.fn) {
            case 0: name = "sll"; break;
            case 2: name = "srl"; break;
            case 3: name = "sra"; break;
            case 8: name = "jr"; break;
            case 12: name = "syscall"; break;
            case 16: name = "mfhi"; break;
            case 18: name = "mflo"; break;
            case 24: name = "mul"; break;
            case 32: name = "add";  break;
            case 34: name = "sub"; break;
            case 36: name = "and"; break;
            case 37: name = "or"; break;
            case 38: name = "xor"; break;
            case 39: name = "nor"; break;
            case 40: name = "slt"; break;
            default: name = NULL; break;
        }

        // Instruction Disassemble
        switch(IR_R_FN_UPPER) {

            case SHIFT:
                switch(IR_R_FN_LOWER) { // set ALU control
                    case SRL: C = ALU_CTRL_SRL; break;
                    case SLL: C = ALU_CTRL_SLL; break;
                    case SRA: C = ALU_CTRL_SRA; break;
                    default: printf("Undefined ALU Control: Shift\n"); return -1;
                }

                X = getRegister(IR.R.rt);
                Y = getRegister(IR.R.sh);
                ret = ALU(X,Y,C,&Z,NULL);
                setRegister(IR.R.rd, ret);
                // REG(IR.R.rd,ret,WRITE);

                printf("%s %s, %s, %d", name, CPU_REG[IR.R.rd], CPU_REG[IR.R.rt], IR.R.sh);
                break;

            case JUMPR_SYSCALL:

                switch(IR_R_FN_LOWER) {
                    case SYSCALL:

                        printf("%s %u",name, getRegister($v0));
                        break;

                    case JR:
                        PC = getRegister($ra);
                        printf("%s %s", name, CPU_REG[IR.R.rs]);
                        break;
                    default: printf("Undefined JUMP_SYSCALL Control\n"); return -1;
                }
                break;

            case MOVE:
                switch(IR_R_FN_LOWER) {
                    case MFHI: setRegister(IR.R.rs,HI); break;
                    case MFLO: setRegister(IR.R.rs,LO); break;
                    default: printf("Undefined MOVE control: Move\n"); return -1;
                }
                printf("%s %s", name, CPU_REG[IR.R.rd]);
                break;

            case MUL: // pseudo instruction
                if(IR_R_FN_LOWER == 0) {
                    unsigned long long result = getRegister(IR.R.rs);
                    result *= getRegister(IR.R.rt);
                    // HI = result >> 16;
                    LO = result & 0xFFFFFFFF;
                    setRegister(IR.R.rs, LO);
                    // REG(IR.R.rs,LO,WRITE);
                    printf("%s %s, %s, %s", name, CPU_REG[IR.R.rs], CPU_REG[IR.R.rs], CPU_REG[IR.R.rt]);
                }
                break;

            case R_ARITH_LOGIC:
                switch(IR_R_FN_LOWER) { // set ALU control
                    case ADD: C = ALU_CTRL_ADD; break;
                    case SUB: C = ALU_CTRL_SUB; break;
                    case AND: C = ALU_CTRL_AND; break;
                    case OR: C = ALU_CTRL_OR; break;
                    case XOR: C = ALU_CTRL_XOR; break;
                    case NOR: C = ALU_CTRL_NOR; break;
                    default: printf("Undefined ALU Control: R_ARITH_LOGIC\n"); return -1;
                }
                // ALU
                X = getRegister(IR.R.rs);
                Y = getRegister(IR.R.rt);
                ret = ALU(X,Y,C,&Z,NULL);
                setRegister(IR.R.rd, ret);
                // REG(IR.R.rd,ret,WRITE);

                printf("%s %s, %s, %s", name, CPU_REG[IR.R.rd], CPU_REG[IR.R.rs], CPU_REG[IR.R.rt]);
                break;

            case SET_LT:
                if(IR_R_FN_LOWER==SLT)
                    C = ALU_CTRL_SLT;
                else {
                    printf("Undefined ALU Control: SLT\n");
                    return -1;
                }
                // ALU
                X = getRegister(IR.R.rs);
                Y = getRegister(IR.R.rt);
                ret = ALU(X,Y,C,&Z,NULL);
                setRegister(IR.R.rd, ret);

                printf("%s %s, %s, %d", name, CPU_REG[IR.R.rd], CPU_REG[IR.R.rs], IR.R.rt);
                break;

            default: printf("R-type fn:%d Not on the Instruction Table!", IR.R.fn); return -1;
        }
        printf("\n");
    } // end of R_type Instruction

    else { // I_Type Instructions
        // Instruction names
        switch(IR.I.op) { // I type Instructions
            case 1: name = "bltz"; break;
            case 2: name = "j"; break;
            case 3: name = "jal"; break;
            case 4: name = "beq"; break;
            case 5: name = "bne"; break;
            case 8: name = "addi"; break;
            case 10: name = "slti"; break;
            case 12: name = "andi"; break;
            case 13: name = "ori";  break;
            case 14: name = "xori"; break;
            case 15: name = "lui"; break;
            case 32: name = "lb"; break;
            case 35: name = "lw"; break;
            case 36: name = "lbu"; break;
            case 40: name = "sb"; break;
            case 43: name = "sw"; break;
            default: name = NULL; break;
        }
        // print formats
        switch(IR_I_OP_UPPER) {

            case JUMP_OR_BRANCH:
                switch(IR_I_OP_LOWER) {
                    case BLTZ:
                        C = ALU_CTRL_SLT;
                        nPC = PC + (IR.I.immi << 2);
                        if(ALU(IR.I.immi, 0, C, &Z, NULL)) { // 0보다 작으면 1
                            setRegister($ra, PC); // set current PC to $ra
                            PC = nPC; // Branch to new PC
                        }
                        printf("%s %s, %d", name, CPU_REG[IR.I.rs], IR.I.immi);
                        break; // end of BLTZ

                    case JAL:
                        setRegister($ra,PC); // set current PC to $ra
                    case JUMP:
                        nPC = (PC & 0xf0000000) | (IR.J.adr << 2);
                        PC = nPC;
                        printf("%s %X", name, nPC);
                        break; // end of JAL or J

                    case BEQ: // advance_pc (offset << 2));
                    case BNE:
                        C = ALU_CTRL_SUB;
                        ret = ALU(IR.I.rs, IR.I.rt, C, &Z, NULL); // if $rs == $rt
                        nPC = PC + (IR.I.immi << 2);
                        if ((Z == 0 && IR_I_OP_LOWER == BEQ) ||
                            (Z != 0 && IR_I_OP_LOWER == BNE)) { // if branch condition is set
                            setRegister($ra, PC); // set current PC to $ra
                            PC = nPC; // Branch to new PC
                        }
                        printf("%s %s, %s, %d", name, CPU_REG[IR.R.rs], CPU_REG[IR.R.rt], IR.I.immi);
                        break;

                    default: printf("Undefined Flow Control: JUMP_OR_BRANCH\n"); return -1;
                } // end of JUMP_OR_BRANCH
                break;

            case I_ARITH_LOGIC:
                switch(IR_I_OP_LOWER) { // set ALU control
                    case ADDI: C = ALU_CTRL_ADD; break;
                    case SLTI: C = ALU_CTRL_SLT; break;
                    case ANDI: C = ALU_CTRL_AND; break;
                    case ORI: C = ALU_CTRL_OR; break;
                    case XORI: C = ALU_CTRL_XOR; break;
                    case LUI: C = ALU_CTRL_SLL; break;  // Load Upper Immediate
                    default: printf("Undefined ALU Control: I_ARITH_LOGIC\n"); return -1;
                }

                if(IR_I_OP_LOWER == LUI) {
                    X = IR.I.immi;
                    Y = 16;
                }
                else {
                    X = getRegister(IR.I.rs);
                    Y = IR.I.immi;
                }
                ret = ALU(X,Y,C,&Z,NULL);
                setRegister(IR.I.rt,ret);


                if(IR_I_OP_LOWER == LUI) // $t = (imm << 16)
                    printf("%s %s, %d", name, CPU_REG[IR.R.rt], IR.I.immi);
                else
                    printf("%s %s, %s, %d", name, CPU_REG[IR.R.rt], CPU_REG[IR.R.rs], IR.I.immi);
                break; // end of I_ARITH_LOGIC

            case LOAD:
            case STORE:

                X = getRegister(IR.I.rs); // base address
                Y = IR.I.immi; // offset
                nAdr = ALU(X,Y,ALU_CTRL_ADD,&Z,NULL); // nAdr = $rs + offset

                if(IR_I_OP_UPPER == LOAD) { // if LOAD
                    switch(IR_I_OP_LOWER) {
                        case LB: size = BYTE; break; // Sign extention required?
                        case LW: size = WORD; break;
                        case LBU: size = BYTE; break;
                        default: printf("Undefined Load Control: LOAD\n"); return -1;
                    }
                    ret = getMemory(nAdr,size);  //ret = MEM[nAdr]
                    setRegister(IR.I.rt,ret);  // $rt = MEM[$rs + offset]

                }else if(IR_I_OP_UPPER == STORE) { // if STORE
                    switch(IR_I_OP_LOWER) {
                        case SB: size = BYTE; break;
                        case LW: size = WORD; break;
                        default: printf("Undefined Store Control: STORE\n"); return -1;
                    }
                    ret = getRegister(IR.I.rt); // ret = $rt
                    // MEM(nAdr,ret,WRITE,WORD);  // MEM[$s + offset] = $t
                    setMemory(nAdr,ret,size);
                }
                printf("%s %s, %d(%s)", name, CPU_REG[IR.R.rt], IR.I.immi, CPU_REG[IR.R.rs]);
                break;
            default: printf("I-type op:%d Not on the Instruction Table!",IR.I.op); return -1;
                // end of LOAD STORE
        }
        printf("\n");
    }
    return IR.INST;
}

