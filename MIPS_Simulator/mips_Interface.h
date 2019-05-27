#ifndef WEEK10_INTERFACE_MIPS_INTERFACE_H
#define WEEK10_INTERFACE_MIPS_INTERFACE_H

#define IF_JUMP_BRANCH (((tmp.I.op & (unsigned)UPPER) >> (unsigned)3) == JUMP_OR_BRANCH)
#define IF_BRANCHED (newPC - oldPC)
#define IF_JR ((((tmp.R.fn & (unsigned)UPPER) >> (unsigned)3) == JUMPR_SYSCALL) && ((tmp.R.fn & (unsigned)LOWER) == JR))

#define INST_MEM 0x400000
#define DATA_MEM 0x400000

#endif //WEEK10_INTERFACE_MIPS_INTERFACE_H
