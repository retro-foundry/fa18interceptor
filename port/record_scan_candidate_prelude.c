#include "record_scan_candidate_prelude.h"

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static int command(const FA18RecordScanCandidateOps *ops, uint16_t first,
                   uint16_t second) {
    return ops && ops->command_dispatch &&
                   ops->command_dispatch(ops->context, first, second) == 0
               ? 0
               : -1;
}

static int stage(const FA18RecordScanCandidateOps *ops,
                 FA18RecordScanIndexStage callback, uint16_t index) {
    return ops && callback && callback(ops->context, index) == 0 ? 0 : -1;
}

static int finish_slot(FA18FlaggedSlot *slot, uint16_t index,
                       const FA18RecordScanCandidateOps *ops,
                       FA18RecordScanCandidateRoute *route) {
    if (!(be16(slot->bytes + 0x26) & 1u)) {
        *route = FA18_RECORD_SCAN_CANDIDATE_ADVANCED;
        return 0;
    }
    if (stage(ops, ops->slot_stage, index) != 0) return -1;
    *route = FA18_RECORD_SCAN_CANDIDATE_SLOT_STAGE;
    return 0;
}

int fa18_run_record_scan_candidate_prelude(
    FA18FlaggedSlot *slot, uint16_t index, FA18RecordScanCandidateState *state,
    const FA18RecordScanCandidateOps *ops,
    FA18RecordScanCandidateRoute *route) {
    uint8_t table_high;
    if (!slot || !state || !ops || !route) return -1;
    if (be16(slot->bytes + 0x26) & 1u)
        return finish_slot(slot, index, ops, route);
    if (state->local_skip) return finish_slot(slot, index, ops, route);
    table_high = state->table_end & 0xf0u;
    if (!table_high || state->renderer_budget < 1) {
        if ((int8_t)state->scan_counter > 0) ++state->local_processed;
        return finish_slot(slot, index, ops, route);
    }
    ++state->local_skip;
    table_high = (uint8_t)(table_high - 0x10u);
    state->table_end = (uint8_t)((state->table_end & 0x0fu) | table_high);
    ++state->local_processed;
    ++state->local_auxiliary;
    state->selected_state_word_3a =
        (uint16_t)(state->selected_state_word_3a + 3u);
    state->state_c457c5 = 1;
    put_be16(slot->bytes + 0x26, 0x0401);
    put_be16(slot->bytes + 0x2e, 0);
    state->local_table_high = table_high;

    if (state->auxiliary_flag) {
        if (!state->flight_update_flag && command(ops, 0x001c, 0x0030) != 0)
            return -1;
        if ((uint8_t)(state->auxiliary_flag - 1u) == 0) {
            put_be16(slot->bytes + 0x26,
                     (uint16_t)(be16(slot->bytes + 0x26) | 0x2000u));
            state->slot_c4588d = (uint8_t)(index + 1u);
        } else {
            put_be16(slot->bytes + 0x26,
                     (uint16_t)(be16(slot->bytes + 0x26) | 0x1000u));
            state->slot_c4588c = (uint8_t)(index + 1u);
        }
        state->auxiliary_flag = 0;
    } else {
        if (!state->flight_update_flag) {
            if (state->renderer_flags & 0x20u) {
                if (command(ops, 0x0006,
                            state->context_selection ? 0x001e : 0x0016) != 0)
                    return -1;
            } else if (command(ops, 0x001c, 0x0030) != 0) {
                return -1;
            }
        }
        state->renderer_budget =
            (int16_t)((uint16_t)state->renderer_budget - 3u);
        if (state->renderer_budget < 0) state->renderer_budget = 0;
        state->mode_c45843 = 3;
        state->scan_counter = 0x14;
    }
    if (stage(ops, ops->indexed_stage, index) != 0) return -1;
    return finish_slot(slot, index, ops, route);
}
