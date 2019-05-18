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

// p is header or footer
// read and write a dword at address p
#define GET(p)          (*(unsigned int *)(p))
#define SET(p, val)     (*(unsigned int *)(p) = (val))

#define GET_SIZE(p)     (GET(p) & ~0x3)
// current block is free
#define IS_FREE(p)      (GET(p) & 0x1)
#define PRE_IS_FREE(p)  (GET(p) & 0x2)

// p is point to the real address of variable
// [HEADER][VARIABLE][FOOTER]
#define HDRP(p)         ((char *)(p) - DWORD_SIZE)
#define FTRP(p)         ((char *)(p) + GET_SIZE(HDRP(p)) - DWORD_SIZE)

typedef struct
{
    __UINT32 size: 30;
    __UINT32 flag: 2; // bit0:是否为空  bit1:前一块是否为空
} BlockHeader;

class MemPool
{
public:
    MemPool();
    ~MemPool();
    
    bool init(size_t size);
    
    bool extend(size_t size);
    
    void* mp_malloc(size_t size);
    
    void mp_free(void* p);
    
protected:
    
private:
    char* header;
    char* tail;
    
    __UINT32 m_dwInitSize;
    
    __UINT32 m_dwExtendStepSize;
    
    __UINT32 m_dwMaxSize;
    
private:
    void mp_merge();
    
    void* find_first_fit(void* p);
    void* find_next_fit(void* p);
    void* find_best_fit(void* p);
    
};

#endif /* MemPool_h */
