#ifndef WEEK11_MIPS_SIM_MIPS_MEMORY_H
#define WEEK11_MIPS_SIM_MIPS_MEMORY_H

#define READ 0
#define WRITE 1

#define MEM_SIZE 0x100000
extern unsigned char progMEM[MEM_SIZE], dataMEM[MEM_SIZE], stakMEM[MEM_SIZE];

unsigned int MEM(unsigned int A, int V, int nRW, int S);
unsigned getMemory(unsigned addr, int S);
int setMemory(unsigned addr, int val, int S);
#endif //WEEK11_MIPS_SIM_MIPS_MEMORY_H
