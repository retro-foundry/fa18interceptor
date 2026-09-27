#ifndef FA18_UPDATE_STAGE_PREFIX_H
#define FA18_UPDATE_STAGE_PREFIX_H

#include <stdint.h>

/* Caller-owned direct state read and written by `$C1C63E-$C1C6BB`. */
typedef struct {
    uint8_t input_byte;
    uint8_t input_byte_mirror;
    uint8_t change_inhibit;
    int32_t signed_long;
    int32_t long_mirror;
    int16_t scaled_word;
    uint8_t mode_byte;
} FA18UpdateStagePrefixState;

typedef int (*FA18IndexedRecordUpdateStage)(void *context);

/* `$C1C63E-$C1C6BB`: synchronize the octant-derived input, update the
 * inverted/scaled long state, and then invoke the required `$C22C80`
 * boundary. `changed` receives D5's source bit-$0b flag. */
int fa18_prepare_update_stage_prefix(FA18UpdateStagePrefixState *state,
                                     FA18IndexedRecordUpdateStage update_stage,
                                     void *context, uint8_t *changed);

#endif
