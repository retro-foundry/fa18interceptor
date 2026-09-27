#ifndef FA18_MAP_PACKET_COORDINATE_SETUP_H
#define FA18_MAP_PACKET_COORDINATE_SETUP_H

#include <stdint.h>

typedef struct {
    uint8_t directory_selector_gate;
    int32_t control_component[3];
    int32_t selector_component[3];
    uint16_t coordinate_bin_shift;
    uint8_t alternate_layout;
} FA18MapPacketCoordinateSetupInput;

typedef struct {
    int16_t origin_component;
    int16_t row_min;
    int16_t column_min;
    int32_t coordinate_x;
    int32_t coordinate_y;
} FA18MapPacketCoordinateSetupResult;

/* `$C2AB7C-$C2ABDD`: gate the directory-selector entry, derive the
 * frame-local origin and packed coordinate terms, then save the two local
 * selector minima.  A nonzero selector gate uses the `$C45C3E` component
 * source before joining the same arithmetic at `$C2AB9C`. */
int fa18_prepare_map_packet_coordinate_setup(
    const FA18MapPacketCoordinateSetupInput *input,
    FA18MapPacketCoordinateSetupResult *result);

#endif
