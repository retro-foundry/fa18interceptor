#include "flagged_slot_scan.h"

int fa18_scan_flagged_flight_slots(const FA18FlaggedSlot slots[FA18_FLAGGED_SLOT_SCAN_SLOTS],
                                   FA18FlaggedSlotEvaluate evaluate, void *context,
                                   int32_t *result) {
    if (!slots || !result) return -1;
    for (int slot = FA18_FLAGGED_SLOT_SCAN_SLOTS - 1; slot >= 0; --slot) {
        if (slots[slot].bytes[0x27] & 1u) {
            if (!evaluate) return -1;
            return evaluate(context, (uint16_t)slot, result);
        }
    }
    *result = 0;
    return 0;
}
