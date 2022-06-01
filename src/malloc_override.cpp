#define GNU_SOURCE

#include <cstdlib>
#include <cstring>
#include <pm/malloc/hook.hpp>

extern "C" void* __libc_malloc(size_t);
extern "C" void  __libc_free(void*);
extern "C" void* __libc_realloc(void*, size_t);

static_assert(sizeof(char) == 1); // sanity

constexpr size_t MEMBLOCK_MAGIC = 0xFEDCBA9876543210;

struct BlockHeader {
    size_t magic;
    size_t size;
};

inline bool is_managed(BlockHeader* block) {
    return (block->magic == MEMBLOCK_MAGIC);
}

extern "C" void* malloc(size_t size) {
    if(!size) return NULL;

    void *ptr = __libc_malloc(size + sizeof(BlockHeader));
    if(!ptr) return ptr; // malloc failed

    auto block = (BlockHeader*)ptr;
    block->magic = MEMBLOCK_MAGIC;
    block->size = size;

    pm::malloc_hook::on_malloc(size);

    return (char*)ptr + sizeof(BlockHeader);
}

extern "C" void free(void* ptr) {
    if(!ptr) return;

    auto block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    if(is_managed(block)) {
        pm::malloc_hook::on_free(block->size);
        __libc_free(block);
    } else {
        __libc_free(ptr);
    }
}

extern "C" void* realloc(void* ptr, size_t size) {
    if(!size) {
        free(ptr);
        return NULL;
    } else if(!ptr) {
        return malloc(size);
    } else {
        auto block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
        if(is_managed(block)) {
            size_t old_size = block->size;
            void *new_ptr = __libc_realloc(block, size + sizeof(BlockHeader));

            auto new_block = (BlockHeader*)new_ptr;
            new_block->magic = MEMBLOCK_MAGIC; // just making sure
            new_block->size = size;

            pm::malloc_hook::on_free(old_size);
            pm::malloc_hook::on_malloc(size);

            return (char*)new_ptr + sizeof(BlockHeader);
        } else {
            return __libc_realloc(ptr, size);
        }
    }
}

extern "C" void* calloc(size_t num, size_t size) {
    size *= num;
    if(!size) return NULL;

    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

// TODO: aligned_alloc
