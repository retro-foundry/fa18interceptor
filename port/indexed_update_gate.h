#ifndef FA18_INDEXED_UPDATE_GATE_H
#define FA18_INDEXED_UPDATE_GATE_H

#include <stdint.h>

typedef enum {
    FA18_INDEXED_UPDATE_ZERO_INDEX_CONTINUATION,
    FA18_INDEXED_UPDATE_ZERO_INDEX_RETURN,
    FA18_INDEXED_UPDATE_ZERO_INDEX_NORMAL,
    FA18_INDEXED_UPDATE_ZERO_INDEX_POSTFLIGHT,
    FA18_INDEXED_UPDATE_NONZERO_INDEX_ROUTE
} FA18IndexedUpdateGateRoute;

/* `$C25B66-$C25B6F`: select the observed indexed-update continuation. */
int fa18_select_indexed_update_gate(uint16_t selected_record_index,
                                    FA18IndexedUpdateGateRoute *route);

/* `$C25B66-$C25B76`: execute the observed zero-index state-byte gate. */
int fa18_run_indexed_update_gate(uint16_t selected_record_index,
                                 uint8_t stride_state_flag,
                                 uint8_t postflight_flag,
                                 FA18IndexedUpdateGateRoute *route);

#endif
