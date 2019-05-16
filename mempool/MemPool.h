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

typedef unsigned int __UINT32;

typedef struct
{
    __UINT32 size: 30;
    __UINT32 flag: 2;
} BlockHeader;

class MemPool
{
public:
    MemPool();
    ~MemPool();
    
    void init(size_t size);
    
    void extend(size_t size);
    
    void* mp_malloc(size_t size);
    
    void mp_free(void * p);
    
protected:
    
private:
    void* header;
    
    __UINT32 m_dwInitSize;
    
    __UINT32 m_dwExtendStepSize;
    
    __UINT32 m_dwMaxSize;
    
private:
    void mp_merge();
    
    void* find_first_fit();
    void* find_next_fit();
    void* find_best_fit();
    
};

#endif /* MemPool_h */
