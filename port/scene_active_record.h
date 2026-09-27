#ifndef FA18_SCENE_ACTIVE_RECORD_H
#define FA18_SCENE_ACTIVE_RECORD_H

#include <stddef.h>
#include <stdint.h>

/* `$C1C54E-$C1C564` forms its active record address as `$C46184 +
 * $C458DE.w`.  The record store and the selected byte offset remain owned by
 * the caller; this view only preserves that source binding and its bounds. */
typedef struct {
    const uint8_t *bytes;
    size_t size;
    size_t base_offset;
    int16_t selected_offset;
} FA18SceneActiveRecordState;

/* Resolve the selected raw record. `minimum_size` is the number of bytes the
 * immediate consumer will read (for `$C1C54E`, it is 0xa4). */
int fa18_resolve_scene_active_record(const FA18SceneActiveRecordState *state,
                                     size_t minimum_size,
                                     const uint8_t **record);

#endif
