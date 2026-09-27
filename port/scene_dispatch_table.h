#ifndef FA18_SCENE_DISPATCH_TABLE_H
#define FA18_SCENE_DISPATCH_TABLE_H

#include <stdint.h>

#include "hunk.h"

enum {
    FA18_SCENE_DISPATCH_TABLE_HUNK = 27,
    FA18_SCENE_DISPATCH_TABLE_OFFSET = 0x01f2,
    FA18_SCENE_DISPATCH_VARIANT_BYTES = 4,
    FA18_SCENE_DISPATCH_MODE_BYTES = 12,
    FA18_SCENE_DISPATCH_RECORD_BYTES = 10
};

/* The relative dispatch data at `$C297D2`, loaded from original Hunk 27.
 * It remains data, never executable 68000 code. */
typedef struct {
    const uint8_t *segment;
    uint32_t segment_size;
    const uint8_t *data;
    uint32_t size;
} FA18SceneDispatchTable;

typedef struct {
    uint8_t mode;
    uint8_t alternate_table;
    uint16_t phase_word;
    uint16_t alternate_variant;
    int16_t mode_list_offset;
} FA18SceneDispatchSelectionInput;

typedef struct {
    const uint8_t *records;
    uint8_t record_type_limit;
} FA18SceneDispatchSelection;

/* One five-word record consumed by `$C28B34`.  The first negative source
 * offset is the enclosing `$C287BE-$C287D8` list terminator. */
typedef struct {
    int16_t source_offset;
    uint8_t record_type;
    uint16_t source_word_1;
    uint16_t geometry_offset;
    uint16_t source_flags;
} FA18SceneDispatchSourceRecord;

/* Five words copied by `MOVEM.W (A4)+,D2-D6` after indexing `$C295E0` with a
 * source record's geometry offset. */
typedef struct {
    int16_t coordinate_x;
    int16_t coordinate_z;
    int16_t component_x;
    int16_t component_z;
    int32_t altitude;
} FA18SceneDispatchGeometry;

/* Bind the `$C297D2` base from verified executable data. */
int fa18_load_scene_dispatch_table(const FA18Hunks *hunks,
                                   FA18SceneDispatchTable *table);

/* `$C2873A-$C287B4`: select a relative record-list variant.  A return of one
 * is the distinct `$7D` route; it is intentionally not treated as a normal
 * table row.  `mode_list_offset` is the caller-owned `$C1AB74` lookup result
 * used by `$C287DA` for modes 1 through 9. */
int fa18_select_scene_dispatch_records(
    const FA18SceneDispatchTable *table,
    const FA18SceneDispatchSelectionInput *input,
    FA18SceneDispatchSelection *selection);

/* Decode one original ten-byte record. Returns one for a record, zero for
 * the negative-offset terminator, and minus one for an invalid table range. */
int fa18_scene_dispatch_source_record(
    const FA18SceneDispatchTable *table, const uint8_t *records,
    uint16_t index, FA18SceneDispatchSourceRecord *record);

/* `$C28C22-$C28C2C`: decode the five-word geometry at `$C295E0 + offset`. */
int fa18_scene_dispatch_geometry(const FA18SceneDispatchTable *table,
                                 uint16_t offset,
                                 FA18SceneDispatchGeometry *geometry);

#endif
