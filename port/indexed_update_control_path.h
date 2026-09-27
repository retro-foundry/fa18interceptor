#ifndef FA18_INDEXED_UPDATE_CONTROL_PATH_H
#define FA18_INDEXED_UPDATE_CONTROL_PATH_H

#include <stdint.h>

typedef enum {
    FA18_INDEXED_UPDATE_MATRIX_DISPATCH_ROUTE,
    FA18_INDEXED_UPDATE_ZERO_INDEX_CONTROL_STAGE,
    FA18_INDEXED_UPDATE_NONZERO_INDEX_CONTINUATION
} FA18IndexedUpdateControlPathRoute;

/* `$C25C54-$C25C69`: route from control bit $0040 and selected index. */
int fa18_route_indexed_update_control_path(
    uint16_t matrix_side_control_flags,
    uint16_t selected_record_index,
    FA18IndexedUpdateControlPathRoute *route);

#endif
