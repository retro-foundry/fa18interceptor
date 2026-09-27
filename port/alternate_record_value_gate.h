#ifndef FA18_ALTERNATE_RECORD_VALUE_GATE_H
#define FA18_ALTERNATE_RECORD_VALUE_GATE_H

#include <stdint.h>

typedef int (*FA18AlternateRecordFixedPoint)(void *context,
                                             const int16_t component[3],
                                             int16_t *value);

typedef struct {
    uint16_t header;
    uint16_t selector_control;
    int16_t record_value;
    int16_t component_word[3];
    int32_t component_long[3];
    FA18AlternateRecordFixedPoint fixed_point;
    void *fixed_point_context;
} FA18AlternateRecordValueGateInput;

typedef struct {
    int16_t record_value;
    int32_t component_long[3];
    uint8_t fixed_point_called;
} FA18AlternateRecordValueGateState;

/* `$C1CEE4-$C1CF35`: publish the long component packet, gate the mutable
 * descriptor value, and conditionally call the source fixed-point boundary. */
int fa18_gate_alternate_record_value(const FA18AlternateRecordValueGateInput *input,
                                     FA18AlternateRecordValueGateState *state);

#endif
