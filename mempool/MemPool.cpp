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
#include "Test.h"

#define theLog (*Logger::getInstance())

MemPool::MemPool()
{
    header = NULL;
    m_pMinRealAddr = NULL;
    m_pMaxRealAddr = NULL;
}

MemPool::~MemPool() 
{
    delete header;
    header = NULL;
}

// 初始化内存，单位为双字（双字=2字=4字节=32位）
bool MemPool::init(__UINT32 dwords)
{
    __UINT32 dwSize = dwords * DWORD_SIZE;
    header = (char*)malloc(dwSize);
    if(NULL == header)
    {
        theLog.error("malloc memory failed.");
        return false;
    }
    theLog.info("malloc memory success.");
    
    // the last DWORD of the memory is the end block
    // so the size of init block is dwSize - DWORD_SIZE
    
    // default assume the last block is in use,
    // so do not need to merge current and the last mem block
    SET(header, PACK(dwSize - DWORD_SIZE, 1, 0));
    SET(FTR(header), PACK(dwSize - DWORD_SIZE, 1, 0));
    
    // the end of the implicit linked memory list
    SET(FTR(header) + DWORD_SIZE, PACK(0, 0, 1));
    
    m_pMinRealAddr = header;
    m_pMaxRealAddr = FTR(header);
    displayMem();
    
    return true;
}

// scale out
bool MemPool::extend(__UINT32 size)
{
    // do nothing
    return true;
}

// find the first fit block begin p, where p is block header
// is not find fit between[p, end], don't serach it in [begin, p)
// return: the header of the fit block
void* MemPool::find_first_fit(void* bp, __UINT32 size)
{
    void* bpAddr = NULL;
    char* bpTmp = (char*)bp;
    while(!IS_END(bpTmp))
    {
        __UINT32 dwOldSize = GET_SIZE(bpTmp);
        if(!USE_FLAG(bpTmp) && dwOldSize >= size + DWORD_SIZE)
        {
            bpAddr = (void*)bpTmp;
            break;
        }
        else
        {
            bpTmp = NEXT(bpTmp);
        }
    }
    return bpAddr;
}

void* MemPool::find_next_fit(void* bp, __UINT32 size)
{
    return NULL;
}

void* MemPool::find_best_fit(void* bp, __UINT32 size)
{
    return NULL;
}

void* MemPool::mp_malloc(__UINT32 size)
{
    void* bpAddr = find_first_fit(header, size);
    if(NULL == bpAddr)
    {
        theLog.error("there is no enough memory.");
        return NULL;
    }
    
    __UINT32 dwOldSize = GET_SIZE(bpAddr);
    __UINT32 dwNewSize = size + DWORD_SIZE;
    __UINT32 dwLeft = dwOldSize - dwNewSize;
    if(dwLeft <= DWORD_SIZE) // the left can al most put a block header, pad it to the malloc mem block
    {
        // if the block header is in use, there will be no block footer(save space)
        SET_PRE_USE_FLAG(bpAddr, 1);
        // set the pre_free_flag of the next block
        SET_PRE_USE_FLAG(NEXT(bpAddr), 1); // code 1 position
    }
    else
    {
        // resize the block header
        // if the block header is in use, there will be no block footer(save space)
        SET(bpAddr, PACK(dwNewSize, !!PRE_USE_FLAG(bpAddr), 1));
        
        // set the free space of the current block(split the current into 2 block)
        // the NEXT(bpTmp) is different from the code 1 position
        SET(NEXT(bpAddr), PACK(dwOldSize - dwNewSize, 1, 0));
        SET(FTR(NEXT(bpAddr)), PACK(dwOldSize - dwNewSize, 1, 0));
    }
    displayMem();
    
    return (void*)REAL(bpAddr);
}

bool MemPool::isValidRealAddress(void* rp)
{
    return true;
}

// rp: the real address of variable
// free a address out of range
// free a address in range, but is block header
// free a address in range, but in the mid of variable range
void MemPool::mp_free(void* rp)
{
    if(!isValidRealAddress(rp))
    {
        theLog.error("try to free an invalid address.");
        return ;
    }
    
    void* new_bp = NULL;
    void* new_next_bp = NULL;
    
    void* bp = HDRP(rp);
    void* next_bp = NEXT(bp);
    
    bool bPreFree = !PRE_USE_FLAG(bp);
    bool bNextFree = !USE_FLAG(next_bp);
    
    // 0. the previous and the next block are both not free
    if(!bPreFree && !bNextFree)
    {
        SET_USE_FLAG(bp, 0);
        new_bp = bp;
        new_next_bp = NEXT(new_bp);

    }
    // 1. the previous and the next block are both free
    else if(bPreFree && bNextFree)
    {
        void* pre_bp = PRE(bp);
        SET(pre_bp, PACK(GET_SIZE(pre_bp) + GET_SIZE(bp) + GET_SIZE(next_bp), !!PRE_USE_FLAG(pre_bp), 0));
        new_bp = pre_bp;
    }
    // 2. the previous block is free
    else if(bPreFree)
    {
        void* pre_bp = PRE(bp);
        SET(pre_bp, PACK(GET_SIZE(pre_bp) + GET_SIZE(bp), !!PRE_USE_FLAG(pre_bp), 0));
        new_bp = pre_bp;
        new_next_bp = NEXT(new_bp);

    }
    // 3. the latter block is free
    else
    {
        SET(bp, PACK(GET_SIZE(next_bp) + GET_SIZE(bp), 1, 0));
        new_bp = bp;
    }
    // set the footer of new merge block
    SET_FTR_BY_HDR(new_bp);
    
    // set the next block of new block
    if(NULL != new_next_bp)
    {
        SET_PRE_USE_FLAG(new_next_bp, 0);
        if(!USE_FLAG(new_next_bp)) // free block need set footer
        {
            SET_FTR_BY_HDR(new_next_bp);
        }
    }
    
    return ;
}

void MemPool::showBlockHeaderInfo(void* bp, const char* szBlockTypeDesc)
{
    char logStr[MAX_LOG_BUFF_LEN] = {0};
    (void)snprintf(logStr, MAX_LOG_BUFF_LEN, "[addr=%p] [val=0x%08x] [%s] [size=%2d] [preinuse=%d] [inuse=%d].", bp, GET(bp), szBlockTypeDesc, GET_SIZE(bp), !!PRE_USE_FLAG(bp), USE_FLAG(bp));
    theLog.info(logStr);
}

void MemPool::displayMem()
{
    /***************************************************************************************************************
    [address] [hex value] [header|footer|variab] [size=%d] [preinuse=true|false] [inuse=true|false]
    ***************************************************************************************************************/
    char* bpTmp = (char*)header;
    
    theLog.info("###########################################################");
    while(!IS_END(bpTmp))
    {
        showBlockHeaderInfo(bpTmp, "header");
        // if the block header is in use, there will be no block footer(save space)
        if(!USE_FLAG(bpTmp))
        {
        	showBlockHeaderInfo(FTR(bpTmp), "footer");
        }
        
        bpTmp = NEXT(bpTmp);
    }
    showBlockHeaderInfo(bpTmp, "   end");
    theLog.info("###########################################################\n");
}

void MemPool::test()
{
    init(45);
    TestObject* obj = NULL;
    //TestObject* obj = (TestObject*)mp_malloc(sizeof(TestObject));
    //printf("%p\n", obj);
    //mp_free(obj);
    
    /*
    TestObject* obj1 = (TestObject*)mp_malloc(sizeof(TestObject));
    printf("%p\n", obj1);
    
    obj = (TestObject*)mp_malloc(sizeof(TestObject));
    printf("%p\n", obj);
    mp_free(obj);
    
    mp_free(obj1);
    */
    
    for(int i = 0; i < 500; i++)
    {
        char logStr[500] = {0};

        if(NULL != (obj = (TestObject*)mp_malloc(sizeof(TestObject))))
        {
            (void)snprintf(logStr, MAX_LOG_BUFF_LEN, "malloc the %d memory block success.", i);
            theLog.info(logStr);
            printf("%p\n", obj);
        }
        else
        {
            (void)snprintf(logStr, MAX_LOG_BUFF_LEN, "malloc the %d memory block failed.", i);
            theLog.error(logStr);
            break;
        }
    }
}



