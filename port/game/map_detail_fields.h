#ifndef FA18_MAP_DETAIL_FIELDS_H
#define FA18_MAP_DETAIL_FIELDS_H

#include <stdint.h>

typedef struct {
    uint8_t alternate_layout;
    uint8_t force_visible;
    uint16_t visibility_gate;
    int32_t detail_metric;
    uint8_t zoom_endpoint;
    int16_t zoom_scale;
    int32_t coordinate_x;
    int32_t coordinate_y;
    int32_t offset_x;
    int32_t offset_y;
} FA18MapDetailFieldsInput;

typedef struct {
    uint16_t visible;
    int32_t coordinate_x;
    int32_t coordinate_y;
    uint8_t visibility_limit_written;
    uint32_t visibility_limit_register;
} FA18MapDetailFieldsResult;

/* `$C2AE5A-$C2AEF7`: apply the map detail gate to prepared coordinate terms,
 * calculate the bounded visibility decision, and format the terms consumed by
 * `$C2AEFC`.  The original lookup addresses preceding image data for a
 * negative metric index; this bounded port rejects that unobserved case. */
int fa18_apply_map_detail_fields(const FA18MapDetailFieldsInput *input,
                                 FA18MapDetailFieldsResult *result);

#endif
