#ifndef FA18_FLAGGED_SLOT_SCAN_H
#define FA18_FLAGGED_SLOT_SCAN_H

#include <stdint.h>

enum { FA18_FLAGGED_SLOT_SCAN_SLOTS = 20 };

typedef int (*FA18FlaggedSlotEvaluate)(void *context, uint16_t slot,
                                       int32_t *result);

typedef struct {
    uint8_t bytes[64];
} FA18FlaggedSlot;

/* `$C265E8-$C26605`: scan slots 19 through zero. A set `+$27` bit zero
 * transfers ownership to the caller-supplied `$C26606` evaluator. */
int fa18_scan_flagged_flight_slots(const FA18FlaggedSlot slots[FA18_FLAGGED_SLOT_SCAN_SLOTS],
                                   FA18FlaggedSlotEvaluate evaluate, void *context,
                                   int32_t *result);

#endif
