#ifndef FA18_MESSAGE_SEQUENCE_H
#define FA18_MESSAGE_SEQUENCE_H

#include <stdint.h>

/* Direct state owned by `$C11312`. The two selector words are deliberately
 * distinct from later sequence population. */
typedef struct {
    uint16_t selector_head[2];
    uint16_t delay;
    uint8_t cursor;
    uint8_t active;
    uint8_t effect_counter;
    uint8_t inhibit;
} FA18MessageSequenceState;

/* `$C11312`: initialize only the observed selector head and control state. */
int fa18_initialize_message_sequence(FA18MessageSequenceState *state);

/* Adapter for ordered caller-owned helper boundaries. */
int fa18_initialize_message_sequence_callback(void *context);

#endif
