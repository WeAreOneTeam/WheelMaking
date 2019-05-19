//
//  main.cpp
//  WheelMaking
//
//  Created by housir on 2019/5/12.
//  Copyright © 2019年 WeAreOneTeam. All rights reserved.
//

#include <iostream>
#include "MemPool.h"
#include "Logger.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, Mem Pool!\n";
    MemPool mp;
    mp.test();
    delete Logger::getInstance();
    return 0;
}
