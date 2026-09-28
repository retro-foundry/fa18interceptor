#ifndef FA18_SCENE_STREAM_ENTRY_H
#define FA18_SCENE_STREAM_ENTRY_H

#include <stddef.h>
#include <stdint.h>

#include "scene_stream_descriptor.h"

typedef struct {
    /* A2 at `$C1EE44`: the selected control stream from descriptor +8. */
    const uint8_t *control_stream;
    size_t control_stream_size;
    uint32_t stream_cursor;
    /* Control-stream base used by its relative branch words. */
    uint32_t control_base;
    /* A0 at `$C1CC6C`: the separate descriptor-record family.  `$C1EF02`
     * indexes this base with the selected low twelve bits; it is not A2. */
    uint32_t descriptor_base;
    int16_t selector_limit;
    int16_t selector_shift;
    uint8_t depth_scale_enabled;
    int16_t depth_scale_word;
    uint8_t activity_flag;
    uint8_t descriptor_gate_flag;
    uint16_t descriptor_gate_word;
    const uint8_t *activity_selector_rows;
    size_t activity_selector_rows_size;
    uint16_t activity_selector_index;
} FA18SceneStreamEntryInput;

typedef enum {
    FA18_SCENE_STREAM_ENTRY_RETURN_ZERO,
    FA18_SCENE_STREAM_ENTRY_RETURN_ONE,
    FA18_SCENE_STREAM_ENTRY_DESCRIPTOR_READY
} FA18SceneStreamEntryRoute;

typedef struct {
    int16_t effective_limit;
    int16_t selected_word;
    FA18SceneStreamDescriptorSetup descriptor;
} FA18SceneStreamEntryResult;

/* `$C1EE14-$C1EF15`: derive the threshold limit, select one source stream
 * record, retain its early return gates, and publish the separate
 * descriptor-record/cursor pair
 * pair consumed by the later `$C1F6F8` walker. The transform and walker
 * bodies remain separate source boundaries. */
int fa18_enter_scene_stream(const FA18SceneStreamEntryInput *input,
                            FA18SceneStreamEntryResult *result,
                            FA18SceneStreamEntryRoute *route);

#endif
