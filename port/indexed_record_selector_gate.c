#include "indexed_record_selector_gate.h"

int fa18_run_indexed_record_selector_gate(
    uint8_t record_offset_20,
    uint8_t record_class,
    uint8_t record_header,
    FA18IndexedRecordSelector selector,
    void *context,
    uint8_t *selector_called) {
    if (!selector || !selector_called) return -1;
    *selector_called = 0;
    if (!(record_offset_20 & 0x02u) && (record_class & 0xf0u) == 0x10u &&
        (record_header & 0x10u)) {
        selector(context);
        *selector_called = 1;
    }
    return 0;
}
