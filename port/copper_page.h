#ifndef FA18_COPPER_PAGE_H
#define FA18_COPPER_PAGE_H

#include <stddef.h>
#include <stdint.h>

#include "video.h"

enum {
    FA18_COPPER_PAGE_PLANES = 5,
    FA18_COPPER_PAGE_ROW_BYTES = FA18_WIDTH / 8,
    FA18_COPPER_PAGE_BYTES = FA18_COPPER_PAGE_ROW_BYTES * FA18_HEIGHT
};

/* A big-endian OCS Copper instruction sequence.  The source page owner
 * provides the lists in execution order; this module never stores a page. */
typedef struct {
    const uint8_t *bytes;
    size_t byte_count;
} FA18CopperInstructionStream;

/* Mutable counterpart for the graphics-library palette-load boundary.  It
 * changes only data words of existing COLORxx Copper MOVEs; the caller owns
 * list allocation, ordering, and the source RGB4 palette. */
typedef struct {
    uint8_t *bytes;
    size_t byte_count;
} FA18CopperMutableInstructionStream;

/* The display state established before a requested Copper vertical position. */
typedef struct {
    uint16_t bplcon0;
    uint32_t plane_pointers[FA18_COPPER_PAGE_PLANES];
    uint16_t palette[32];
    uint32_t palette_written_mask;
} FA18CopperPageState;

/* Caller-owned native backing for one original pointer value.  The pointer is
 * an identity key from the Copper stream, not a host address to dereference. */
typedef struct {
    uint32_t source_pointer;
    const uint8_t *bytes;
    size_t byte_count;
} FA18CopperPlaneBuffer;

/* Decode the `$C07F00 -> $C555F8 -> $C55680`-shaped OCS Copper-list
 * boundary.  `$C07F00` is a graphics.library `CopList` in the observed
 * runtime; its dynamic list/pointer owner remains outside this decoder.
 *
 * Only ordinary MOVE and WAIT instructions are accepted.  A Copper SKIP or
 * malformed sequence returns -1 rather than guessing its display state. */
int fa18_decode_copper_page_at_vpos(const FA18CopperInstructionStream *streams,
                                    size_t stream_count, uint8_t vpos,
                                    FA18CopperPageState *state);

/* Apply selected RGB4 words to existing COLOR00--COLOR31 Copper MOVE data
 * words across caller-provided lists.  This is the native form of the
 * observed `$C085F0 -> $0577B0` palette-load boundary; it never creates a
 * list, assumes a captured palette, or changes Copper register words. */
int fa18_update_copper_palette_moves(FA18CopperMutableInstructionStream *streams,
                                     size_t stream_count,
                                     const uint16_t palette[32],
                                     uint32_t palette_mask,
                                     uint32_t *updated_mask);

/* Present a five-plane `$5200` state through caller-owned page buffers. The
 * source's big-endian bit order is preserved. A complete 32-register Copper
 * palette state replaces the video palette; an incomplete list leaves the
 * caller palette intact rather than filling missing entries with guesses. */
int fa18_present_copper_page(const FA18CopperPageState *state,
                             const FA18CopperPlaneBuffer *buffers,
                             size_t buffer_count, FA18Video *video);

#endif
