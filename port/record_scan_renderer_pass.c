#include "record_scan_renderer_pass.h"

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

int fa18_run_record_scan_renderer_pass(
    FA18FlaggedSlot slots[FA18_FLAGGED_SLOT_SCAN_SLOTS],
    const FA18RecordScanRendererPassInput *input,
    FA18RecordScanRendererPassState *state,
    FA18RecordScanRendererPassRoute *route) {
    if (!slots || !input || !state || !route) return -1;
    state->renderer_bound = UINT32_C(0x000fffff);
    state->scan_counter = (uint8_t)(state->scan_counter - 1u);
    /* `$C15200-$C1522D`: a set mode byte bypasses the `$C15218` flags test,
     * then falls into the direct advance when the auxiliary byte is clear. */
    if (input->auxiliary_flag ||
        (!input->scan_mode && (input->flags & 0x0008u))) {
        *route = FA18_RECORD_SCAN_RENDERER_PASS_CANDIDATE_CONTINUATION;
        return 0;
    }
    for (unsigned index = 0; index < FA18_FLAGGED_SLOT_SCAN_SLOTS; index += 2) {
        FA18FlaggedSlot *slot = &slots[index];
        if (!input->skip_countdown)
            put_be16(slot->bytes + 0x28,
                     (uint16_t)(be16(slot->bytes + 0x28) - 1u));
        if (be16(slot->bytes + 0x26) & 0x0001u) {
            *route = FA18_RECORD_SCAN_RENDERER_PASS_CANDIDATE_CONTINUATION;
            return 0;
        }
    }
    state->scan_counter = 0;
    state->table_end = (uint8_t)((state->table_end & 0x0fu) | 0x50u);
    *route = FA18_RECORD_SCAN_RENDERER_PASS_COMPLETE;
    return 0;
}
