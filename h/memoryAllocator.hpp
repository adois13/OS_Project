#ifndef _memory_allocator_hpp_
#define _memory_allocator_hpp_

#include "hw.h"

class MemoryAllocator {
private:
    MemoryAllocator();
    
    struct BlockHeader {
        size_t size;
        BlockHeader* next;
        BlockHeader(size_t size, BlockHeader* next) : size(size), next(next) {}
    };

    BlockHeader* freeBlocksHead;
    BlockHeader* usedBlocksHead;

    int tryToJoin(BlockHeader*);

public:
    MemoryAllocator(const MemoryAllocator&) = delete;
    static MemoryAllocator* getMemoryAllocator();

    void* _malloc(size_t size);
    int _free(void*);

};

#endif