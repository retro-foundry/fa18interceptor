#ifndef FA18_FLAGGED_SLOT_EVALUATOR_H
#define FA18_FLAGGED_SLOT_EVALUATOR_H

#include <stdint.h>

#include "flagged_slot_scan.h"
#include "scene_component_magnitude.h"

typedef struct {
    uint8_t bytes[512];
} FA18FlaggedLinkedRecord;

typedef int (*FA18FlaggedLinkedRecordResolve)(
    void *context, int16_t signed_byte_offset,
    const FA18FlaggedLinkedRecord **record);

typedef struct {
    const FA18FlaggedSlot *slots;
    const FA18SceneMagnitudeTable *magnitude_table;
    FA18FlaggedLinkedRecordResolve resolve_linked_record;
    void *resolve_context;
    int32_t origin_x;
    int32_t origin_y;
    int32_t origin_z;
    uint8_t context_selection;
} FA18FlaggedSlotEvaluator;

/* `$C26606-$C266AB`: evaluate one flagged 64-byte slot. The resolver owns the
 * source `$C46184` table, whose signed byte offset is formed with word-sized
 * `ASL #8; ADD; ADDA.W` arithmetic. */
int fa18_evaluate_flagged_flight_slot(void *context, uint16_t slot,
                                      int32_t *result);

#endif
