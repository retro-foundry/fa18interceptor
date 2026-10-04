#ifndef AMIGA_HUNK_LOADER_H
#define AMIGA_HUNK_LOADER_H
#include "hunk.h"
#include "guest_memory.h"
typedef struct { uint32_t payload_base, allocation_size; } AmigaHunkPlacement;
/* The profile owns placement; the reusable loader owns validation, DOS segment
 * headers, BSS clearing and relocation. No captured memory is loaded. */
int amiga_hunks_install(const AmigaHunks *, const AmigaHunkPlacement *, size_t,
    const AmigaGuestMemory *, uint32_t *segment_list_bptr, char *error, size_t error_size);
#endif
