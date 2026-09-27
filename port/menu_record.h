#ifndef FA18_MENU_RECORD_H
#define FA18_MENU_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "hunk.h"

/* `$C3ED0A` is offset +$0A in CODE hunk 64 of the disk executable. */
enum { FA18_MENU_TEXT_HUNK = 64, FA18_MENU_RECORD_TABLE_OFFSET = 0x0a };

typedef struct {
    const uint8_t *text;
    size_t text_length;
    uint32_t layout_offset;
    uint8_t attribute;
    uint8_t layout_index;
} FA18MenuRecord;

/* `$C32D24` positive-selector path. Negative selectors take a distinct
 * original path and are intentionally rejected until it is ported. */
int fa18_menu_select_message_record(const FA18Hunks *exe, uint16_t selector,
                                    FA18MenuRecord *record);

#endif
