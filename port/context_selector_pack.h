#ifndef FA18_CONTEXT_SELECTOR_PACK_H
#define FA18_CONTEXT_SELECTOR_PACK_H

#include <stddef.h>
#include <stdint.h>

enum { FA18_CONTEXT_SELECTOR_RECORD_BYTES = 0x6e };

typedef struct {
    uint8_t context_selection;
    uint8_t mode;
    int32_t projection_depth;
    int32_t origin[3];
    uint16_t selector_word_x;
    uint16_t selector_word_y;
    uint8_t selector_byte_x;
    uint8_t selector_byte_y;
    uint8_t selector_change;
    uint8_t magnitude_class;
} FA18ContextSelectorPackState;

/* `$C29042` remains caller-owned, but the callback makes its mutable origin
 * output available before the `$C1C6BC` alternate selector calculations. */
typedef int (*FA18ContextSelectorOriginUpdate)(void *context,
                                               int32_t origin[3]);

/* `$C1C6BC-$C1C7F5`: classify the selected record and update the second
 * selector pack. `prior_change` is D5 inherited from `$C1C63E`; its bits are
 * ORed into `$C45858` with alternate-route changes. */
int fa18_update_context_selector_pack(
    const uint8_t *record, size_t record_size,
    FA18ContextSelectorPackState *state, uint8_t prior_change,
    FA18ContextSelectorOriginUpdate update_origin, void *context);

#endif
