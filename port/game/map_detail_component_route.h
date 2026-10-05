#ifndef FA18_MAP_DETAIL_COMPONENT_ROUTE_H
#define FA18_MAP_DETAIL_COMPONENT_ROUTE_H

#include <stdint.h>

/* `$C2AEF8-$C2AEFB`: combine the completed D0/D1 coordinate terms into the
 * packed D0 seed consumed by `$C2AF92`'s static-pair transform. */
uint32_t fa18_complete_map_detail_component_route(int32_t coordinate_x,
                                                  int32_t coordinate_y);

#endif
