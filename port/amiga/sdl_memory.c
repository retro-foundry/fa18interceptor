#include "sdl_memory.h"
#include <SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
typedef __declspec(align(16)) struct { unsigned char bytes[16]; } PoolAlignment;
#else
typedef max_align_t PoolAlignment;
#endif
typedef union Block Block;
union Block {
    PoolAlignment alignment;
    struct { size_t bytes; Block *next,*previous; int used; } value;
};
static union { PoolAlignment alignment; unsigned char bytes[AMIGA_SDL_MEMORY_BYTES]; } arena;
static SDL_SpinLock lock;
static AmigaSdlMemoryStats stats;
static int installed;

static void *allocate(size_t bytes) {
    const size_t alignment=sizeof(PoolAlignment);
    ++stats.requests;
    if(!bytes) bytes=1;
    if(bytes>SIZE_MAX-alignment+1) {++stats.failures;return NULL;}
    bytes=(bytes+alignment-1)/alignment*alignment;
    for(Block *block=(Block *)arena.bytes;block;block=block->value.next) {
        if(block->value.used || block->value.bytes<bytes) continue;
        if(block->value.bytes-bytes>=sizeof(Block)+alignment) {
            Block *rest=(Block *)((unsigned char *)(block+1)+bytes);
            rest->value.bytes=block->value.bytes-bytes-sizeof(Block);
            rest->value.used=0;rest->value.previous=block;rest->value.next=block->value.next;
            if(rest->value.next) rest->value.next->value.previous=rest;
            block->value.next=rest;block->value.bytes=bytes;
        }
        block->value.used=1;stats.used+=block->value.bytes;
        if(stats.used>stats.peak) stats.peak=stats.used;
        return block+1;
    }
    ++stats.failures;return NULL;
}
static void release(void *pointer) {
    if(!pointer) return;
    Block *block=(Block *)pointer-1;
    if(!block->value.used) abort();
    block->value.used=0;stats.used-=block->value.bytes;
    Block *next=block->value.next;
    if(next && !next->value.used) {
        block->value.bytes+=sizeof(Block)+next->value.bytes;block->value.next=next->value.next;
        if(block->value.next) block->value.next->value.previous=block;
    }
    Block *previous=block->value.previous;
    if(previous && !previous->value.used) {
        previous->value.bytes+=sizeof(Block)+block->value.bytes;previous->value.next=block->value.next;
        if(previous->value.next) previous->value.next->value.previous=previous;
    }
}
static void *SDLCALL pool_malloc(size_t bytes) {
    SDL_AtomicLock(&lock);void *result=allocate(bytes);SDL_AtomicUnlock(&lock);return result;
}
static void SDLCALL pool_free(void *pointer) {
    SDL_AtomicLock(&lock);release(pointer);SDL_AtomicUnlock(&lock);
}
static void *SDLCALL pool_calloc(size_t count,size_t bytes) {
    if(bytes && count>SIZE_MAX/bytes) {
        SDL_AtomicLock(&lock);++stats.requests;++stats.failures;SDL_AtomicUnlock(&lock);return NULL;
    }
    void *result=pool_malloc(count*bytes);
    if(result) memset(result,0,count*bytes);
    return result;
}
static void *SDLCALL pool_realloc(void *pointer,size_t bytes) {
    if(!pointer) return pool_malloc(bytes);
    if(!bytes) {pool_free(pointer);return NULL;}
    SDL_AtomicLock(&lock);
    Block *block=(Block *)pointer-1;
    void *result=pointer;
    if(bytes>block->value.bytes) {
        result=allocate(bytes);
        if(result) {memcpy(result,pointer,block->value.bytes);release(pointer);}
    }
    SDL_AtomicUnlock(&lock);return result;
}
int amiga_sdl_memory_install(void) {
    if(installed) return 1;
    memset(arena.bytes,0,sizeof arena.bytes);
    Block *first=(Block *)arena.bytes;first->value.bytes=sizeof arena.bytes-sizeof *first;
    if(SDL_SetMemoryFunctions(pool_malloc,pool_calloc,pool_realloc,pool_free)) return 0;
    installed=1;return 1;
}
AmigaSdlMemoryStats amiga_sdl_memory_stats(void) {
    SDL_AtomicLock(&lock);AmigaSdlMemoryStats result=stats;SDL_AtomicUnlock(&lock);return result;
}
