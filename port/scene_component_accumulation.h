#ifndef FA18_SCENE_COMPONENT_ACCUMULATION_H
#define FA18_SCENE_COMPONENT_ACCUMULATION_H

#include <stddef.h>
#include <stdint.h>

/* The `$C46184` family is selected by the high byte of the descriptor header.
 * Only the three longwords read by `$C1D0B6` are represented here. */
typedef struct {
    uint32_t word_14;
    int32_t word_18;
    uint32_t word_1c;
} FA18SceneComponentRecord;

typedef struct {
    int32_t component[3];
    uint8_t accumulated;
} FA18SceneComponentAccumulation;

/* `$C1D0B6-$C1D100`: prepare descriptor-indexed components.  The source
 * masks only the two horizontal source longwords to 20 bits before applying
 * the header's low-nibble arithmetic shift. */
int fa18_accumulate_scene_components(uint16_t descriptor_header,
                                     const FA18SceneComponentRecord *records,
                                     size_t record_count,
                                     int32_t input_x,
                                     int32_t input_z,
                                     int32_t middle_bias,
                                     FA18SceneComponentAccumulation *output);

#endif
