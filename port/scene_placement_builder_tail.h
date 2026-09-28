#ifndef FA18_SCENE_PLACEMENT_BUILDER_TAIL_H
#define FA18_SCENE_PLACEMENT_BUILDER_TAIL_H

#include <stddef.h>
#include <stdint.h>

#include "scene_placement.h"

/* Caller-owned inputs published by `$C1DDCA/$C1DE04/$C1DE38` and the three
 * normalized magnitudes reaching `$C1DF04`. */
typedef struct {
    int32_t coordinate_work[3];
    int16_t magnitude[3];
    const uint8_t *shift_table;
    size_t shift_table_size;
    uint32_t record_tail;
    uint8_t cycle_byte;
} FA18ScenePlacementBuilderTailInput;

typedef struct {
    uint8_t shift_count;
    uint8_t next_cycle_byte;
} FA18ScenePlacementBuilderTailResult;

typedef struct {
    int16_t workspace_word[2];
    int16_t correction_word[2];
    int32_t translated_component[2];
    int32_t origin_component[2];
    uint8_t append_enabled;
} FA18ScenePlacementWorkInput;

/* `$C1DD98-$C1DE38`: form the first and third per-record work longwords.
 * The caller supplies the selected workspace pair, C1D7E2 correction pair,
 * and the live translated/origin terms established earlier in the builder. */
int fa18_build_scene_placement_work(const FA18ScenePlacementWorkInput *input,
                                    int32_t work[3]);

/* `$C1DF04-$C1E0B0`: select the adaptive precision shift, OR it into the
 * low selector byte at `+1`, store the three arithmetic-shifted coordinate
 * words at `+6..+11`, then write the exact mutable record suffix. */
int fa18_finish_scene_placement_record(
    uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES],
    const FA18ScenePlacementBuilderTailInput *input,
    FA18ScenePlacementBuilderTailResult *result);

#endif
