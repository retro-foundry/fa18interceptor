#ifndef FA18_MAP_DETAIL_FIELDS_H
#define FA18_MAP_DETAIL_FIELDS_H

#include <stdint.h>

typedef int (*FA18MapVisibilityLimitResolver)(void *context,int16_t index,uint16_t *limit);
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
    /* Original C2AE88 also reads preceding image words for negative indices. */
    FA18MapVisibilityLimitResolver resolve_visibility_limit;
    void *visibility_context;
} FA18MapDetailFieldsInput;

typedef struct {
    /* Source selector word: forced 1, compared X's low word, or 0. */
    uint16_t visible;
    int32_t coordinate_x;
    int32_t coordinate_y;
    uint8_t visibility_limit_written;
    uint32_t visibility_limit_register;
} FA18MapDetailFieldsResult;

/* `$C2AE5A-$C2AEF7`: apply the map detail gate to prepared coordinate terms,
 * calculate the bounded visibility decision, and format the terms consumed by
 * `$C2AEFC`. A loaded-image resolver is required for negative metric indices;
 * ordinary nonnegative entries can use the reconstructed eighteen-word table. */
int fa18_apply_map_detail_fields(const FA18MapDetailFieldsInput *input,
                                 FA18MapDetailFieldsResult *result);

#endif
