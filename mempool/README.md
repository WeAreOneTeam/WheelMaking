# 场景

1. 频繁的申请和释放内存
2. 内存大小不定
3. 多线程

# 方案

使用隐式空闲链表
按照四字节对齐

BlockHeader
{
    uint size:30; // size << 2表示此块的大小
    uint flag:2; // bit0:是否为空  bit1:前一块是否为空
};

空闲的块首位都有一个BlockHeader，方便合并前面的块
正在使用的块只有首部有一个BlockHeader

# 关键词：
block header: 内存块头
real address: 变量使用的真正地址，即内存块头+DWORD_SIZE
