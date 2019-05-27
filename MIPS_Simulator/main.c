#include <stdio.h>
#include <string.h>
#include "mips_header.h"
#include "mips_Interface.h"
#include "mips_Disassembly.h"
#include "mips_Register.h"
#include "mips_Memory.h"

unsigned oldPC, newPC;

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

unsigned int endian_swap(unsigned l, int S) { // little-endian 에서 big-endian으로
    if(S == WORD) {
        l = (l>>24) |
            ((l<<8) & 0x00FF0000) |
            ((l>>8) & 0x0000FF00) |
            (l<<24);
    }
    else if(S == HALF_WORD) {
        l = (l>>8) | (l<<8);
    }
    return l;
}

int if_exit(union Instruction tmp) {
    unsigned IR_R_FN_LOWER = tmp.R.fn & (unsigned) LOWER,
            IR_R_FN_UPPER = (tmp.R.fn & (unsigned) UPPER) >> (unsigned) 3;
    if (IR_R_FN_UPPER == JUMPR_SYSCALL && IR_R_FN_LOWER == SYSCALL) {
        if (getRegister($v0) == 10) {
            printf("finished!\n\n");
            fflush(stdout);
            return 1;
        }
    }
          return 0;
}


unsigned step(void) {
    union Instruction tmp;
    oldPC = PC;
    tmp.INST = get_Instruction(PC,0);
    newPC = PC;
    if(IF_JR || !(IF_JUMP_BRANCH && IF_BRANCHED)) {
        PC += 4; // increase PC by 4
    }
    /* PC increment exceptions
     * BEQ/BNE/BLTZ
     * if condition is met, advance_pc (offset << 2)); else advance_pc (4);
     *  - don't advance pc by 4 if condition is met
     *
     *  JAL/J
     *  - don't advance pc by 4
     */
    return tmp.INST;
}

// print out memory with start & offset(words)
/*void viewMemory(unsigned start, unsigned num) {
    *//* start: Base address of memory to read
     * offset: number of bytes to read
     *//*

    printf("------------------[MEMORY]------------------\n");
    // end = start + offset-> instsize와 같이 명령어 개수로! (alligned access 실행됨!)
    int i=0;
    unsigned current = start+(i<<2);
    while(i<num) {
        for(int j=i%8;j<8;++i,++j) {
        `   current = start+(i<<2);
            if(i == num) break;
            printf("%X ", getMemory(start+(i<<2),WORD));

        }
        printf(" ");
        if((start+i)%4 == 0) printf("\n");
    }
}*/

// print out memory with start/end address
void viewMemory(unsigned start, unsigned end) {
    /* start: Base address of memory to read
     * end: end address
     */
    printf("------------------[MEMORY]------------------\n");
    // end = start + offset-> instsize와 같이 명령어 개수로! (alligned access 실행됨!)
    int i=0;
    unsigned current = start+(i<<2);
    while(current<=end) {
        for(int j=i%8;j<8;++i,++j) { // print eight data in a row
            current = start+(i<<2);
            if(current > end) break;
            printf("%8.8X ", getMemory(current,WORD));
        }
        printf(" ");
        if(current%4 == 0) printf("\n");
    }
    fflush(stdout);
}

void viewRegister(void) {
    printf("------------------[REGISTER]------------------\n");
    for(int i=0;i<8;++i) {
        for (int j = 0; j < 8*4; j+=8) { // print eight data in a row
            printf("%s = %8.8X  ", CPU_REG[i + j], getRegister(i+j));
        }
        printf("\n");
    }
    printf("\nPC: %X\t",PC);
    printf("HI/LO: %X %X\n\n",HI,LO);
    fflush(stdout);
}

void menu(char* command) {
    printf("l:  Load program\n"
           "g:  Go\n"
           "s:  Step\n"
           "m:  view Memory\n"
           "r:  view Register\n"
           "sm: set Memory\n"
           "sr: set Register\n"
           "x:  exit program\n");
    fflush(stdout);
    scanf("%s", command); //Get command line;
    // clear_stdin();
}

FILE* load() {
    const int MAX_PATH = 100;
    unsigned int data_size, inst_size;
    char path[MAX_PATH];
    unsigned val;

    // clear path buffer
    memset(path,0,MAX_PATH);
    // reset Memory
    memset(progMEM,0,MEM_SIZE);
    memset(dataMEM,0,MEM_SIZE);
    memset(stakMEM,0,MEM_SIZE);

    // initialize Registers
    HI = 0, LO = 0;
    setPC(PC_INIT);
    setRegister($sp, SP_INIT); // SP 초기화

    // Load read file to memory
    printf("Enter File Path >> ");
    fflush(stdout);
    fgets(path,MAX_PATH,stdin);
    strlen(path);
    path[strlen(path)-1] = '\0';

    FILE* fp= fopen(path, "rb");
    if(fp == NULL) perror("file load error!\n");
    // get number of instructions & data
    inst_size =number_of_Inst(fp);
    data_size = number_of_Data(fp);
    // data, program size 만큼 dataMEM, progMEM에 쓴다.
    fseek(fp,8,SEEK_SET); // Instruction 위치에 file offset 위치

    /*long pos = 2 * sizeof(int) + inst_size + data_size; // number of data/instructions + instruction pos
    fseek(fp, pos, SEEK_SET);*/
    for(int i=0;i<inst_size;++i) { // load instructions to instruction Memory
        fread(&val,sizeof(unsigned),1,fp); // 워드 단위로 읽는다
        MEM(((unsigned)INST_MEM+(i<<2)),val,WRITE,WORD);
    }

    for(int i=0;i<data_size;++i) { // load data to data Memory
        fread(&val,sizeof(unsigned),1,fp); // 워드 단위로 읽는다
        MEM(((unsigned)DATA_MEM+(i<<2)),val,WRITE,WORD);
    }
    return fp;
}

int main() { // run_simulator 함수로
    char command[5];
    static int EXIT = 0;
    FILE* fp = load();

    while(1) {

        menu(command);
        clear_stdin();

        if(strcmp(command,"l") == 0) { // load program
            fp = load();
            if(fp == NULL) {
                perror("Load Error!\n");
                continue;
            }

        } else if(strcmp(command,"j") == 0) { // jump to address
            unsigned int adr;
            scanf("%x",&adr);
            setPC(adr);

        } else if(strcmp(command,"g") == 0) { // run untill syscall 10 or breakpoint
            do {
                IR.INST = step();
                EXIT = if_exit(IR);
            } while(!EXIT); // syscall 10 or breakpoint까지 실행
            fclose(fp);

        } else if(strcmp(command,"s") == 0) { // step

            IR.INST = step();
            if_exit(IR);
            fclose(fp);

        } else if(strcmp(command,"m") == 0) { // view memory

            unsigned int start, num, end;
            printf("Enter (Start Address) (End Address) >> ");
            fflush(stdout);
            scanf("%x %x",&start,&end);
            viewMemory(start, end);

        } else if(strcmp(command,"r") == 0) { // view register
            viewRegister();

        } else if(strcmp(command,"sm") == 0) { // set memory
            // MEM은 endian swap 해서 집어넣어야 -> 사용자 입력을 받을 때에는 setMemory 이용
            unsigned addr, val, size;
            printf("0: BYTE, 1: Half Word, 2: WORD >> ");
            fflush(stdout);
            scanf("%d",&size);
            printf("Enter (Address) (Value) >> ");
            fflush(stdout);
            scanf("%x %u",&addr,&val);
            setMemory(addr,val,size);

        } else if(strcmp(command,"sr") == 0) { // set register
            // REG는 endian swap 해서 집어넣어야 -> 사용자 입력을 받을 때에는 setRegister 이용
            unsigned no, val;
            printf("Enter (No) (Value) >> ");
            fflush(stdout);
            scanf("%u %x",&no,&val);

            if(setRegister(no, val) == 0)
                printf("setRegisterError");

        } else if(strcmp(command,"x") == 0) // exit
            return 1;
        else {
            printf("Undefined command!\n");
            fflush(stdout);
        }
        fflush(stdout);

    }
}