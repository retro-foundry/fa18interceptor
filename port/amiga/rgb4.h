#ifndef AMIGA_RGB4_H
#define AMIGA_RGB4_H
#include <stddef.h>
#include <stdint.h>

/* Caller-owned colour-map data, in the original big-endian RGB4 format.
 * Raw words are retained here; display-list writes use their low 12 bits. */
typedef struct { uint8_t *bytes; size_t byte_count; } AmigaRgb4Palette;
typedef struct {
    /* Six-byte CopIns records: opcode, register/wait position, data/mask. */
    uint8_t *instructions;
    size_t byte_count, instruction_count;
    /* Optional actual merged-list owner. Called after the internal data word
     * changes, in instruction order. Failure retains preceding writes. */
    int (*write_hardware)(void *context, size_t instruction, uint16_t value);
    void *context;
} AmigaRgb4CopperList;
typedef struct { uint8_t *bytes; size_t byte_count; } AmigaRgb4HardwareList;

/* Copy the leading count words, with overlap defined by memmove. These two
 * stages let the packed compatibility adapter resolve its list after copying,
 * retaining its established partial-write order. Return nonzero on success. */
int amiga_rgb4_copy(AmigaRgb4Palette *palette, const uint8_t *source,
                      size_t source_bytes, size_t count);
int amiga_rgb4_patch(const AmigaRgb4Palette *palette, size_t count,
                       AmigaRgb4CopperList *list);
/* Count clips to the actual colour-map capacity. A missing list means that
 * the viewport has no display list yet, as in the existing host service. */
int amiga_rgb4_load(AmigaRgb4Palette *palette, const uint8_t *source,
                      size_t source_bytes, size_t count, AmigaRgb4CopperList *list);
/* Actual ordinary-buffer owner for the four-byte merged Copper list. */
int amiga_rgb4_write_hardware(void *context, size_t instruction, uint16_t value);
#endif
