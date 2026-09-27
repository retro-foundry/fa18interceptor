#ifndef FA18_INDEXED_UPDATE_GATE_H
#define FA18_INDEXED_UPDATE_GATE_H

#include <stdint.h>

typedef enum {
    FA18_INDEXED_UPDATE_ZERO_INDEX_ROUTE,
    FA18_INDEXED_UPDATE_NONZERO_INDEX_ROUTE
} FA18IndexedUpdateGateRoute;

/* `$C25B66-$C25B6F`: select the observed indexed-update continuation. */
int fa18_select_indexed_update_gate(uint16_t selected_record_index,
                                    FA18IndexedUpdateGateRoute *route);

#endif
