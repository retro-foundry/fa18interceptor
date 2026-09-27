#ifndef FA18_HUNK_H
#define FA18_HUNK_H

#include <stddef.h>
#include <stdint.h>

/* The game executable as loaded from the disk: its CODE/DATA/BSS segments and
 * their 32-bit relocations. The port reads tables and text from it; it never
 * executes the 68000 code. */
typedef enum { FA18_HUNK_CODE, FA18_HUNK_DATA, FA18_HUNK_BSS } FA18HunkKind;

typedef struct {
    uint32_t offset;   /* field offset inside this segment */
    uint16_t target;   /* segment the stored offset points into */
} FA18HunkReloc;

typedef struct {
    FA18HunkKind kind;
    uint8_t *data;
    uint32_t size;
    FA18HunkReloc *relocs; /* sorted by offset */
    uint32_t reloc_count;
} FA18HunkSegment;

typedef struct {
    FA18HunkSegment *segments;
    uint32_t count;
} FA18Hunks;

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
