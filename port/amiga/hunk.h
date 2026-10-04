#ifndef AMIGA_HUNK_H
#define AMIGA_HUNK_H

#include <stddef.h>
#include <stdint.h>

/* Disk-format values and guest addresses; no host or SDK structure layout. */
typedef enum { AMIGA_HUNK_CODE, AMIGA_HUNK_DATA, AMIGA_HUNK_BSS } AmigaHunkKind;
typedef struct { uint32_t offset; uint16_t target; } AmigaHunkReloc;
typedef struct {
    AmigaHunkKind kind;
    uint8_t *data;
    uint32_t size;
    AmigaHunkReloc *relocs;
    uint32_t reloc_count;
    uint32_t memory_flags; /* HUNK header bits 31:30: any/chip/fast. */
} AmigaHunkSegment;
typedef struct { AmigaHunkSegment *segments; uint32_t count; } AmigaHunks;

int amiga_hunks_parse(AmigaHunks *, const uint8_t *, size_t);
void amiga_hunks_free(AmigaHunks *);
int amiga_hunk_pointer(const AmigaHunks *, uint32_t, uint32_t, uint32_t *, uint32_t *);
static inline uint16_t amiga_be16(const uint8_t *p) { return (uint16_t)(p[0] << 8 | p[1]); }
static inline uint32_t amiga_be32(const uint8_t *p) {
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}
#endif
