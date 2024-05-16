#include "../h/memoryAllocator.hpp"

MemoryAllocator *MemoryAllocator::getMemoryAllocator()
{
    static MemoryAllocator memoryAllocator;
    return &memoryAllocator;
}

MemoryAllocator::MemoryAllocator()
{
    freeBlocksHead = (BlockHeader*)((uint64)HEAP_START_ADDR);
    freeBlocksHead -> size = ((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR - sizeof(BlockHeader)) / MEM_BLOCK_SIZE;
    freeBlocksHead -> next = nullptr;
    usedBlocksHead = nullptr;
}

void* MemoryAllocator::_malloc(size_t size) // size-broj blokova za alokaciju
{
    BlockHeader* curr = freeBlocksHead, *prev = nullptr;
    while(curr) {
        if(curr -> size >= size) break;
        prev = curr;
        curr = curr -> next;
    }

    if (curr == nullptr) return nullptr;

    if (curr->size > size)
    {
        BlockHeader* newFree = (BlockHeader*)((uint64)curr + size * MEM_BLOCK_SIZE);
        newFree->size = curr->size - size;
        newFree->next = curr->next;
        curr->next = newFree;
    }

    if(prev) { 
        prev -> next = curr -> next; 
    } else { 
        freeBlocksHead = curr->next;
    }

    BlockHeader* newBlock = (BlockHeader*)curr;
    newBlock -> size = size; 
    newBlock -> next = nullptr;
    
    if(!usedBlocksHead) {
        usedBlocksHead = newBlock;
        usedBlocksHead -> next = nullptr;
    } else if ((uint64)newBlock < (uint64)usedBlocksHead) {
        newBlock -> next = usedBlocksHead;
        usedBlocksHead = newBlock;
    } else {
        prev = usedBlocksHead;
        while(prev -> next && (uint64)(prev -> next) < (uint64)curr) {
            prev = prev -> next;
        }
        newBlock -> next = prev -> next;
        prev -> next= newBlock;
    }
    // else {
    //     prev = usedBlocksHead;
    //     while(prev -> next && prev -> next < curr) {
    //         prev = prev -> next;
    //     }
    //     if(prev) {
    //         newBlock->next = prev->next;
    //         prev -> next = newBlock;
    //     } else {
    //         newBlock -> next = usedBlocksHead;
    //         usedBlocksHead = newBlock;
    //     }
    // }

    return (void*)((uint64)newBlock + sizeof(BlockHeader));
}

int MemoryAllocator::_free(void* addr)
{
    if(!addr) return 0;

    BlockHeader* toFree = (BlockHeader*)((uint64)addr - sizeof(BlockHeader));
    BlockHeader* curr = usedBlocksHead, *prev = nullptr;

    while(curr) {
        if(curr == toFree) {
            if(prev) {
                prev -> next = curr -> next;
            } else {
                usedBlocksHead = curr -> next;
            }
            break;
        }
        prev = curr;
        curr = curr -> next;
    }

    if (!curr) return -1;
    toFree->next = nullptr;

    if (toFree < freeBlocksHead)
    {
        toFree->next = freeBlocksHead;
        freeBlocksHead = toFree;
    }
    else {
        curr = freeBlocksHead, prev = nullptr;
        for(; curr; prev = curr, curr = curr -> next){
            if((uint64)curr + curr -> size <= (uint64)toFree && 
            (uint64)toFree + toFree->size < (uint64)(curr -> next)) {
                    toFree -> next = curr -> next;
                    if (prev)
                        prev -> next = toFree;
                    else if(freeBlocksHead == curr) {
                        prev = freeBlocksHead;
                        freeBlocksHead -> next = toFree;
                    }
                    break;
            }
        }
    }

    tryToJoin(prev);
    tryToJoin(curr);
    return 0;
}

int MemoryAllocator::tryToJoin(BlockHeader *block)
{
    if(!block) return 0;    
    
    if (block -> next && (uint64)block + block->size*MEM_BLOCK_SIZE == (uint64)(block->next)){
        block -> size += block -> next -> size;
        block -> next = block -> next -> next;
        return 1;
    } else {
        return 0;
    }
}
