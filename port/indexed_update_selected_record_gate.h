#ifndef FA18_INDEXED_UPDATE_SELECTED_RECORD_GATE_H
#define FA18_INDEXED_UPDATE_SELECTED_RECORD_GATE_H

#include <stdint.h>

typedef enum {
    FA18_INDEXED_UPDATE_SELECTOR_GATE_ROUTE,
    FA18_INDEXED_UPDATE_SELECTED_RECORD_CONTINUATION
} FA18IndexedUpdateSelectedRecordRoute;

/* `$C25D22-$C25D3F`: compare indices, context, and selected record +3 bit 0. */
int fa18_route_indexed_update_selected_record(
    uint16_t selected_record_index,
    uint16_t reference_record_index,
    uint8_t indexed_update_context,
    uint8_t record_offset_3,
    FA18IndexedUpdateSelectedRecordRoute *route);

#endif
