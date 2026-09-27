#ifndef FA18_INDEXED_RECORD_SELECTOR_GATE_H
#define FA18_INDEXED_RECORD_SELECTOR_GATE_H

#include <stdint.h>

typedef void (*FA18IndexedRecordSelector)(void *context);

/* `$C25D5E-$C25D85`: conditionally call `$C13D84`; both paths reach `$C25D86`. */
int fa18_run_indexed_record_selector_gate(
    uint8_t record_offset_20,
    uint8_t record_class,
    uint8_t record_header,
    FA18IndexedRecordSelector selector,
    void *context,
    uint8_t *selector_called);

#endif
