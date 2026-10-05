#ifndef FA18_MAP_DETAIL_GATE_H
#define FA18_MAP_DETAIL_GATE_H

#include <stdint.h>

typedef struct {
    uint8_t frame_flag;
    uint8_t alternate_layout;
    uint8_t shared_gate;
    uint8_t shared_detail_byte;
    int32_t metric;
} FA18MapDetailGateInput;

typedef enum {
    FA18_MAP_DETAIL_GATE_READY = 0,
    FA18_MAP_DETAIL_GATE_TERMINATOR = 1,
    FA18_MAP_DETAIL_GATE_FRAME_STOP = 2
} FA18MapDetailGateRoute;

typedef struct {
    uint8_t mode;
    uint8_t frame_flag;
    uint8_t detail_byte;
    uint16_t visibility_flag;
    uint16_t coordinate_shift;
} FA18MapDetailGateResult;

/* `$C2AD00-$C2AD7F`: decode one signed control mode and derive the three
 * caller-visible detail fields consumed by `$C2AE5A` and `$C2AF92`. */
int fa18_select_map_detail_gate(int8_t encoded_mode,
                                const FA18MapDetailGateInput *input,
                                FA18MapDetailGateResult *result,
                                FA18MapDetailGateRoute *route);

#endif
