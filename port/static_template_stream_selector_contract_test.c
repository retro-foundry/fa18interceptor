#include "static_template_stream_selector.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t immutable[96] = {0};
    uint8_t bitset[16] = {0};
    uint8_t pairs[32] = {0};
    uint8_t workspace[0x600] = {0};
    uint8_t first_records[0x200 * 16] = {0};
    uint8_t second_records[0x20 * 16] = {0};
    FA18TemplateWorkspaceAppend append = {
        first_records, sizeof first_records, second_records, sizeof second_records,
        0, 0, 1
    };
    FA18StaticTemplateStreamInput input = {
        immutable, sizeof immutable, 0, bitset, sizeof bitset, pairs, sizeof pairs,
        workspace, sizeof workspace, &append
    };
    FA18StaticTemplateStreamResult result;
    FA18StaticTemplateStreamRoute route;

    immutable[2] = 0x00; immutable[3] = 0x20;
    immutable[0x20] = 0x00; immutable[0x21] = 0x04;
    immutable[0x22] = 0x00; immutable[0x23] = 0x07;
    immutable[0x24] = 0x00; immutable[0x25] = 0x09;
    immutable[0x26] = 0x00; immutable[0x27] = 0x00;
    immutable[0x28] = 0x00; immutable[0x29] = 0x40;
    immutable[0x2a] = 0x00; immutable[0x2b] = 0x00;
    immutable[0x2c] = 0x00; immutable[0x2d] = 0x50;
    immutable[0x40] = 0x02; immutable[0x41] = 0x6e;
    immutable[0x42] = 0x08; immutable[0x43] = 0x00;
    immutable[0x44] = 0x08; immutable[0x45] = 0x00;
    immutable[0x46] = 0x13; immutable[0x47] = 0x80;
    immutable[0x48] = 0xff;
    pairs[2] = 0x80; pairs[3] = 0x22;
    bitset[8] = 0x00; bitset[9] = 0x00; bitset[10] = 0x00; bitset[11] = 0x80;
    first_records[1] = 0x50;
    first_records[7] = 0x07;
    first_records[9] = 0x01;
    first_records[10] = 0x02;

    assert(fa18_select_static_template_stream(&input, 1, 7, &result, &route) == 0);
    assert(route == FA18_STATIC_TEMPLATE_STREAM_EXPANDED);
    assert(result.selected_row_index == 0 && result.expanded_item_count == 2);
    assert(!memcmp(workspace + 0xc0, (uint8_t[]){0, 0x6e, 8, 0, 8, 0, 0xff}, 7));
    assert(!memcmp(workspace + 0xc7, (uint8_t[]){0x10, 0, 0xff}, 3));
    assert(result.append_marker_count == 2 && !(first_records[1] & 0x10));
    assert(!memcmp(workspace + 0x120, (uint8_t[]){0x80, 0, 0xff, 0x80, 0xff, 0x22, 0xff}, 7));

    bitset[11] = 0;
    assert(fa18_select_static_template_stream(&input, 1, 7, &result, &route) == 0 &&
           route == FA18_STATIC_TEMPLATE_STREAM_REJECTED_BIT);
    bitset[10] = 0x01; bitset[11] = 0x80;
    assert(fa18_select_static_template_stream(&input, 1, 8, &result, &route) == 0 &&
           route == FA18_STATIC_TEMPLATE_STREAM_REJECTED_ROW && result.status_word == 0x1c);
    immutable[0x20] = 0xff;
    assert(fa18_select_static_template_stream(&input, 1, 7, &result, &route) == 0 &&
           route == FA18_STATIC_TEMPLATE_STREAM_REJECTED_GROUP);
    return 0;
}
