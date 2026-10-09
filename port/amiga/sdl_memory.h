#ifndef AMIGA_SDL_MEMORY_H
#define AMIGA_SDL_MEMORY_H
#include <stddef.h>
enum { AMIGA_SDL_MEMORY_BYTES=32*1024*1024 };
typedef struct {
    size_t used,peak,requests,failures;
} AmigaSdlMemoryStats;
/* Call before any SDL initialization/allocation. All SDL-owned C storage then
 * comes from one fixed, startup-touched arena, including event/render caches.
 * Driver/OS allocations are outside SDL's allocator interface. */
int amiga_sdl_memory_install(void);
AmigaSdlMemoryStats amiga_sdl_memory_stats(void);
#endif
