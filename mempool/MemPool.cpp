//
//  MemPool.cpp
//  WheelMaking
//
//  Created by housir on 2019/5/17.
//  Copyright © 2019年 housir. All rights reserved.
//

#include "MemPool.h"
#include <stdlib.h>
#include "Logger.h"

#define theLog (*Logger::getInstance())

MemPool::MemPool()
{
    header = NULL;
}

MemPool::~MemPool() 
{
    delete header;
    header = NULL;
}

// 初始化内存，单位为双字（双字=2字=4字节=32位）
bool MemPool::init(size_t dwords)
{
    __UINT32 dwSize = (__UINT32)dwords * DWORD_SIZE;
    header = (char*)malloc(dwSize);
    if(NULL == header)
    {
        theLog.error("malloc memory failed.");
        return false;
    }
    theLog.info("malloc memory success.");
    
    // 隐式链表头
    //BlockHeader& bh = *((BlockHeader*)header);
    //bh.size = dwSize | 0x2; // 默认前一块不为空，这样就不需要合并，方便处理
    SET(HDRP(header), PACK(dwSize, 1, 0));
    
    // 隐式链表尾
    //BlockHeader& bt = *((BlockHeader*)(header + dwSize - DWORD_SIZE));
    //bt.size = dwSize * DWORD_SIZE | 0x2; // 默认前一块不为空，这样就不需要合并，方便处理
    SET(FTRP(header), PACK(dwSize, 1, 0));
    
    return true;
}

// scale out
bool MemPool::extend(size_t size)
{
    // do nothing
    return true;
}

void MemPool::mp_merge()
{
    
}

void* MemPool::find_first_fit(void* p)
{
    
    return NULL;
}

void* MemPool::find_next_fit(void* p)
{
    return NULL;
}

void* MemPool::find_best_fit(void* p)
{
    return NULL;
}

void* MemPool::mp_malloc(size_t size)
{
    return NULL;
}

void MemPool::mp_free(void* p)
{
}



