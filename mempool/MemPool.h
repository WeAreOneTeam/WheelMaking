//
//  MemPool.h
//  WheelMaking
//
//  Created by housir on 2019/5/17.
//  Copyright © 2019年 housir. All rights reserved.
//

#ifndef MemPool_h
#define MemPool_h

#include <stdio.h>

#include "commondefine.h"

#define PACK(size, pre_empty, inuse) ((size) | ((pre_empty)<<1) | (inuse))

/**************** p is point the header of mem block ****************/
// read and write a dword at address p
#define GET(p)                      (*(unsigned int *)(p))
#define SET(p, val)                 (*(unsigned int *)(p) = (val))

// bit 0
#define USE_FLAG(p)                (GET(p) & 0x1)
#define SET_USE_FLAG(p, flag)      (SET(p, ((GET(p) & ~0x1) | flag)))
// bit 1
#define PRE_USE_FLAG(p)            (GET(p) & 0x2)
#define SET_PRE_USE_FLAG(p, flag)  (SET(p, ((GET(p) & ~0x2) | (flag << 1))))
// bit 2~31
#define GET_SIZE(p)                 (GET(p) & ~0x3)
#define SET_SIZE(p, size)           (SET(p, ((GET(p) & 0x3) | size)))

// whether the block p is the end of the memory pool
// the end block header: size = 0 and flag bit0 is 1, the bit1 depends
// attention: the end of the mem pool, inuse = 1
#define IS_END(p)         ((GET(p) & ~0x2) == 0x1)

// the real address of variable
#define REAL(p)            ((char *)(p) + DWORD_SIZE)
#define FTR(p)             ((char *)(p) + GET_SIZE(p) - DWORD_SIZE)
#define NEXT(p)            ((char *)(p) + GET_SIZE(p))
// the footer of the previous block
#define PRE_FTR(p)         ((char *)(p) - DWORD_SIZE)
// the header of the previous block
#define PRE(p)             ((char *)(p) - GET_SIZE(PRE_FTR(p)))

#define SET_FTR_BY_HDR(bp) (GET(FTR(bp)) = GET(bp))

/**************** p is point to the real address of variable ****************/
// [HEADER][VARIABLE][FOOTER]
#define HDRP(rp)           ((char *)(rp) - DWORD_SIZE)

// 位域的比特序：分配位域存储地址(字节内bit地址)，从字节内 低bit地址开始分配
typedef struct
{
    __UINT32 dwUseFlag: 1;    // bit0: whether the current block is in use
    __UINT32 dwPreUseFlag: 1; // bit1: whether the pre block is in use
    __UINT32 dwSize: 30;
} BlockHeader;

class MemPool
{
public:
    MemPool();
    ~MemPool();
    
    bool init(__UINT32 size);
    
    bool extend(__UINT32 size);
    
    void* mp_malloc(__UINT32 size);
    void mp_free(void* rp);
    
    void showBlockHeaderInfo(void* bp, const char* szBlockTypeDesc);
    void displayMem();
    
    void test();
    
protected:
    
private:
    char* header;
    
    // the range of real address
    void* m_pMinRealAddr;
    void* m_pMaxRealAddr;
    
private:    
    void* find_first_fit(void* bp, __UINT32 size);
    void* find_next_fit(void* bp, __UINT32 size);
    void* find_best_fit(void* bp, __UINT32 size);
    
    bool isValidRealAddress(void* rp);
    
};

#endif /* MemPool_h */
