#ifndef FA18_ALTERNATE_FLIGHT_SCALE_H
#define FA18_ALTERNATE_FLIGHT_SCALE_H

#include "scene_component_accumulation.h"

typedef int (*FA18AlternateFlightAccumulate)(void *context, uint16_t header,
                                             const int16_t tuple[3],
                                             int32_t component[3]);

typedef struct {
    uint16_t header;
    int16_t tuple[3];
    const FA18SceneComponentRecord *component_records;
    size_t component_record_count;
    int32_t middle_bias;
    FA18AlternateFlightAccumulate alternate_accumulate;
    void *alternate_context;
} FA18AlternateFlightScaleInput;

typedef enum {
    FA18_ALTERNATE_FLIGHT_SCALE_VALUE_GATE,
    FA18_ALTERNATE_FLIGHT_SCALE_FIXED_POINT
} FA18AlternateFlightScaleRoute;

typedef struct {
    int16_t component_word[3];
    int32_t component_long[3];
    uint8_t component_ready;
} FA18AlternateFlightScaleState;

/* `$C1CEA4-$C1CEE3`: select direct scaling, `$C1D0B6` accumulation, or the
 * `$C1D0A4` alternate-helper boundary, then publish the source word/long
 * component packets. */
int fa18_scale_alternate_flight_record(const FA18AlternateFlightScaleInput *input,
                                       FA18AlternateFlightScaleState *state,
                                       FA18AlternateFlightScaleRoute *route);

#endif
