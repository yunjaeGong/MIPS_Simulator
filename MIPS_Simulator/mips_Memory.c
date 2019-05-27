#include <stdio.h>
#include <stdlib.h>
#include "mips_header.h"
#include "mips_Memory.h"


unsigned char progMEM[0x100000], dataMEM[0x100000], stakMEM[0x100000];

/*int main() {
    short data_hW[] = {0x1234, 0x2345, 0x0FFF, 0x0001,0x5EDC};
    int data_W[] = {0x10000000, 0x12345678, 0x98765432, 0x00FFFFFF, 0x0000FFFF};
    char data_B[] = {0x12, 0x34, 0x0E, 0x01, 0x0F};

    unsigned int Address [3][5] = { {0x400013, 0x400020, 0x400058, 0x400ABC, 0x40FFFA},         // Program
                                  {0x10000013, 0x10000020, 0x10000058, 0x10000ABC, 0x1000FFFA},   // Data
                                  {0x7FF00013, 0x7FF00020, 0x7FF00058, 0x7FF00ABC, 0x7FF0FFFA} }; // Stack
    // 데이터를 저장할 주소들

    void* data_arr[] = {data_B, data_hW, data_W};

    for(int i=0;i<3;++i) { // i = Size
        printf("-------------write-------------\n");
        Size = i;
        for(int j=0;j<5;j++) {
            int V;
            switch(Size) { // 자료 크기 따라 메모리에 쓸 값을 V에 저장
                case BYTE: V = ((char*)data_arr[i])[j]; break;
                case HALF_WORD: V = (short)endian_swap(((unsigned short*)data_arr[i])[j], HALF_WORD); break;
                case WORD: V = (int)endian_swap(((unsigned int*)data_arr[i])[j], WORD); break;
                default: V = -1; break;
            }
            printf("Written data in DEC: %d\n",V); // 쓸 데이터 출력
            if (V==-1) return -1;
            MEM(Address[i][j],V,1,i); // 메모리에 쓴다.
        }
    } // Write to Mem

    int output;
    for(int i=0;i<3;++i) {
        printf("-------------read-------------\n");
        for(int j=0;j<5;j++) {
            Size = i;
            output = MEM(Address[i][j],0,0,i); // Address 배열에 저장된 주소에 저장된 값을 읽어온다.
            switch(Size) { // 메모리에서 읽어온 데이터 출력
                case BYTE: printf("%hhi\n",(char)output); break;
                case HALF_WORD: printf("%hi\n",(short)output); break;
                case WORD: printf("%d\n",output); break;
            }
        }

    } // Read from Mem
    return 0;
}*/

unsigned int MEM(unsigned int A, int V, int nRW, int S) {
    /*
     * A: Memory Address
     * V: Write value
     * nRW: 0 -> read, 1 -> write
     * S(Size): 0 -> Byte, 1 -> Half Word, 2 -> Word
     */
    unsigned int sel, offset;
    unsigned char *pM;
    sel = A >> 20; //
    offset = A & 0xFFFFF;
    if (sel == 0x004) pM = progMEM;        // program memory
    else if (sel == 0x100) pM = dataMEM;  // data memory
    else if (sel == 0x7FF) pM = stakMEM;  // stack
    else {
        printf("No memory\n");
        exit(1);
    }

    /* Alligned memory access
     *
     * Half_Word -> Address [31:1] + 0
     * Word      -> Address [31:2] + 00
     *
     * 구현 방법:
     *
     * Half_Word:
     * A = 0b1111 1111 이라 가정 Aligned Access를 위해서는 LSB 1이 0이 되어야한다.
     * 최하위 비트를 0으로 만들기 위해 A >> 1 연산 수행 -> 0b1111 111
     * 이후 다시 왼쪽 shift 연산 수행(A << 1) -> 0b1111 1110
     *
     * Word:
     * 위와 같은 방법 이용. 왼쪽, 오른쪽으로 2 bit만큼 shift
     * A = 0b1010 1111 이라 가정, Aligned Access를 위해서는 하위 두 bit 11이 00이 되어야한다.
     * 00으로 만들기 위해 A >> 2 연산 수행 -> 0b1010 11
     * 이후 다시 왼쪽 shift 연산 수행(A << 2) -> 0b1010 1100
     */

    if (S == BYTE) {  // byte
        if (nRW == READ) { // read from mem
            return *((unsigned char*)pM + offset);

        } else if (nRW == WRITE) { // write to mem
            *((unsigned char*)pM + offset) = (char)V; // 저장하는 자료형(Byte)에 맞는 포인터 pm + offset 연산 결과를 참조한 위치에 값을 쓴다.
        }
    } else if (S == HALF_WORD) { // Half Word
        if (nRW == READ) { // read from mem
            return *((unsigned short*)pM + (offset>>1)); // 입력된 주소에 저장된 데이터를 반환

        } else if (nRW == WRITE) { // write to mem
            unsigned long tmp = (unsigned)((unsigned short *)pM + (offset>>1));
            tmp = (tmp >> 1) << 1;  //  shift 연산을 통해 Aligned Access 구현
            *(unsigned short*)tmp = (unsigned short)V; // Aligned 연산 위치에 데이터 V 저장
        }
    } else if (S == WORD) { // Word

        if (nRW == 0) { // read from mem
            return *((unsigned int*)pM + (offset>>2)); // 입력된 주소에 저장된 데이터를 반환

        } else if (nRW == WRITE) { // write to mem
            unsigned long tmp = (unsigned)((unsigned int*)pM + (offset>>2)); // Aligned Memory Access를 위해 데이터를 저장할 위치의 주소를 저장할 변수 tmp
            tmp = (tmp >> 2) << 2;  //  shift 연산을 통해 Aligned Access 구현
            *(unsigned int*)tmp = (unsigned int)V;  // Aligned 연산 위치에 데이터 V 저장
        }
    } else {
        printf("Size out of boundary\n");
        exit(1);
    }
    return 0;
}

unsigned getMemory(unsigned addr, int S) {
    // get Memory with address, Size
    return endian_swap(MEM(addr,0,READ,S),S); // big to little
}

int setMemory(unsigned addr, int val, int S) { // unsigned로 sign extend 되고 endian swap 됨. user input용도
    // set Memory with address, value. Size
    val = endian_swap(val,S); // little to big endian
    return MEM(addr,val,WRITE,S);
}