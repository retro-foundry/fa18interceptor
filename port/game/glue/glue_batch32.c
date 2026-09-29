/* Glue for fault_hook $C06C02 and file_records_by_level $C1D5D8. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "fault.h"
#include "globals.h"
#include "memory.h"

/* $C06C02: RTS. */
int glue_C06C02(void) {
    fault_hook();
    return glue_return();
}

/* $C1D5D8: D3.w column, D2.w row, A1 the lists, A2 the list position.
 * The caller reads D1 (low byte the last level, $FF after the bank
 * switch; high word level * 32) and D6 (the record index). */
int glue_C1D5D8(void) {
    FilingState s;
    int enabled = rd_u8(CELL_CHECKS) != 0;
    s.level = -1;
    s.level_offset = (int16_t)(D(1) >> 16);
    s.index = (int)D(6);
    s.cursor = A(2);
    file_records_by_level((int16_t)D(3), (int16_t)D(2), A(1), &s);
    if (enabled) {
        D(1) = (uint32_t)(uint16_t)s.level_offset << 16 | (D(1) & 0xFF00u) | (uint8_t)s.level;
        D(6) = (uint32_t)s.index;
    }
    return glue_return();
}
