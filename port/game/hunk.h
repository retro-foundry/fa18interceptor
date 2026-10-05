#ifndef FA18_HUNK_H
#define FA18_HUNK_H

#include <stddef.h>
#include <stdint.h>
#include "../amiga/hunk.h"

/* The game executable as loaded from the disk: its CODE/DATA/BSS segments and
 * their 32-bit relocations. The port reads tables and text from it; it never
 * executes the 68000 code. */
typedef AmigaHunkKind FA18HunkKind;
typedef AmigaHunkReloc FA18HunkReloc;
typedef AmigaHunkSegment FA18HunkSegment;
typedef AmigaHunks FA18Hunks;
#define FA18_HUNK_CODE AMIGA_HUNK_CODE
#define FA18_HUNK_DATA AMIGA_HUNK_DATA
#define FA18_HUNK_BSS AMIGA_HUNK_BSS

int fa18_hunks_load(FA18Hunks *hunks, const uint8_t *file, size_t size);
void fa18_hunks_free(FA18Hunks *hunks);

/* A pointer stored in segment `seg` at `offset`: its target segment and the
 * offset inside it. Returns 0 when that field is not relocated. */
int fa18_hunk_pointer(const FA18Hunks *hunks, uint32_t seg, uint32_t offset,
                      uint32_t *target_seg, uint32_t *target_offset);

static inline uint16_t fa18_be16(const uint8_t *p) { return (uint16_t)(p[0] << 8 | p[1]); }
static inline uint32_t fa18_be32(const uint8_t *p) {
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

#endif
