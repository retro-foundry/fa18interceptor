#include "menu_record.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    uint8_t data[32] = {0};
    /* Selector 1 points to a descriptor at table + 6. */
    data[FA18_MENU_RECORD_TABLE_OFFSET] = 0;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 1] = 6;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 2] = 0x7f;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 3] = 0xff;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 6] = 4;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 7] = 0xfe;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 8] = 3;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 9] = 0x52;
    memcpy(data + FA18_MENU_RECORD_TABLE_OFFSET + 10, "MENU", 5);
    FA18HunkSegment segments[FA18_MENU_TEXT_HUNK + 1] = {{0}};
    segments[FA18_MENU_TEXT_HUNK] = (FA18HunkSegment){.data = data, .size = sizeof data};
    FA18Hunks exe = {.segments = segments, .count = FA18_MENU_TEXT_HUNK + 1};
    FA18MenuRecord record;
    if (fa18_menu_select_message_record(&exe, 1, &record) != 0 ||
        record.text_length != 4 || memcmp(record.text, "MENU", 4) != 0 ||
        record.layout_offset != 158 || record.attribute != 3 || record.layout_index != 5) {
        fputs("message record positive selector contract failed\n", stderr);
        return 1;
    }
    if (fa18_menu_select_message_record(&exe, 0, &record) != -1 ||
        fa18_menu_select_message_record(&exe, 2, &record) != -1 ||
        fa18_menu_select_message_record(NULL, 1, &record) != -1) {
        fputs("message record bounds contract failed\n", stderr);
        return 1;
    }
    memcpy(data + FA18_MENU_RECORD_TABLE_OFFSET + 10, "OLD", 4);
    data[FA18_MENU_RECORD_TABLE_OFFSET + 14] = 4;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 15] = 0;
    data[FA18_MENU_RECORD_TABLE_OFFSET + 16] = 0xa2;
    memcpy(data + FA18_MENU_RECORD_TABLE_OFFSET + 17, "NEXT", 5);
    record = (FA18MenuRecord){.text = data + FA18_MENU_RECORD_TABLE_OFFSET + 10,
                              .text_length = 3, .layout_offset = 0x19ca};
    if (fa18_menu_select_inline_followup(&exe, &record, 0x1e0, &record) != 0 ||
        record.text_length != 4 || memcmp(record.text, "NEXT", 4) != 0 ||
        record.layout_offset != 0x1baa || record.attribute != 0 ||
        record.layout_index != 10) {
        fputs("message record inline followup contract failed\n", stderr);
        return 1;
    }
    puts("message record selector contract passed");
    return 0;
}
