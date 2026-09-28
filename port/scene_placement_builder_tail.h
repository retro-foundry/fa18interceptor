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

/* Inputs still live at `$C1DE3E` after the two workspace work values have
 * been formed.  Descriptor components are the raw words at +$0c/+0e/+10;
 * the source masks them to twelve bits before adding them. */
typedef struct {
    int32_t coordinate_work[3];
    int16_t projection_packet[2];
    int32_t projection_depth;
    uint16_t descriptor_component[3];
    uint8_t header_flags;
} FA18ScenePlacementMagnitudeInput;

/* `$C1DD98-$C1DE38`: form the first and third per-record work longwords.
 * The caller supplies the selected workspace pair, C1D7E2 correction pair,
 * and the live translated/origin terms established earlier in the builder. */
int fa18_build_scene_placement_work(const FA18ScenePlacementWorkInput *input,
                                    int32_t work[3]);

/* `$C1DE3E-$C1DF03`: derive the three normalized magnitudes consumed by
 * `$C1DF04`.  Descriptor additions occur only when header bits 4 or 6 select
 * either source descriptor route. */
int fa18_derive_scene_placement_magnitudes(
    const FA18ScenePlacementMagnitudeInput *input, int16_t magnitude[3]);

typedef struct { uint16_t selector_word; uint8_t descriptor_index; } FA18ScenePlacementHeader;
/* `$C1DD36-$C1DD88` observed direct route (bit 7 of the low header byte clear). */
int fa18_decode_scene_placement_workspace_header(const uint8_t workspace[2],
                                                 FA18ScenePlacementHeader *header);

/* `$C1DF04-$C1E0B0`: select the adaptive precision shift, OR it into the
 * low selector byte at `+1`, store the three arithmetic-shifted coordinate
 * words at `+6..+11`, then write the exact mutable record suffix. */
int fa18_finish_scene_placement_record(
    uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES],
    const FA18ScenePlacementBuilderTailInput *input,
    FA18ScenePlacementBuilderTailResult *result);

/* `$C1DD36-$C1DD88` followed by `$C1E042-$C1E0B0`: emit a record from
 * already-published work values.  Flagged descriptor routes retain the prior
 * third scratch value, so they cannot use `$C1DD98-$C1DE38`'s payload helper. */
int fa18_finish_scene_placement_record_from_work(
    const uint8_t workspace_item[2], uint32_t descriptor_reference,
    const FA18ScenePlacementBuilderTailInput *tail,
    uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES],
    FA18ScenePlacementBuilderTailResult *result);

typedef int (*FA18ScenePlacementDescriptorResolve)(void *, uint8_t, uint32_t *);
typedef struct {
    const uint8_t *workspace_item;
    const FA18ScenePlacementWorkInput *work;
    FA18ScenePlacementBuilderTailInput tail;
    FA18ScenePlacementDescriptorResolve resolve;
    void *context;
    /* `$C1DDE8` retains a prior third scratch word, so its caller has already
     * formed all three `$C456EE/$F2/$F6` values and the descriptor reference. */
    uint8_t use_published_work;
    uint32_t published_descriptor_reference;
} FA18ScenePlacementBuildInput;
/* Compose the observed direct header/work/tail route into one 24-byte record. */
int fa18_build_scene_placement_record(const FA18ScenePlacementBuildInput *, uint8_t[FA18_SCENE_PLACEMENT_BYTES], FA18ScenePlacementBuilderTailResult *);

#endif
