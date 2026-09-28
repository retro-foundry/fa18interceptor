#include "record_stream_selector.h"

#include <assert.h>

int main(void) {
    const uint8_t bytes[] = {
        0,0, 0,0, 0,0, 0,0, 0x80,0x12, 0,0, 0,0, 0,0,
        0x12,0x34, 0x56,0x78, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    FA18RecordStreamSelectorInput input = {bytes, sizeof bytes, 0x1000, 0x1010, (int16_t)0x8008};
    FA18RecordStreamSelectorResult result;
    FA18RecordStreamSelectorRoute route;

    assert(fa18_select_record_streams(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_STREAM_SELECTOR_DISPATCH &&
           result.a1_cursor == 0x100a && result.a2_address == 0x1012 &&
           result.a1_flag == 1 && !result.a1_count && !result.a2_count);

    const uint8_t direct_a1_bytes[] = {
        0,0, 0,0, 0,0, 0,0, 0x80,0x12, 0,0, 0,0, 0,0,
        0,0, 0x10,8
    };
    input.stream_bytes = direct_a1_bytes; input.stream_size = sizeof direct_a1_bytes;
    input.control_word = (int16_t)0x9000;
    assert(fa18_select_record_streams(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_STREAM_SELECTOR_DISPATCH &&
           result.next_control_cursor == 0x1014 && result.a1_cursor == 0x100a &&
           result.a2_address == 0x1012 && result.a1_flag == 1);

    input.stream_bytes = bytes; input.stream_size = sizeof bytes;
    input.control_word = (int16_t)0xa000;
    assert(fa18_select_record_streams(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_STREAM_SELECTOR_DISPATCH && result.a2_count == 1 &&
           result.a2_address == 0x1000 && result.next_control_cursor == 0x1010);

    input.control_word = (int16_t)0xb000;
    assert(fa18_select_record_streams(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_STREAM_SELECTOR_DISPATCH && result.a2_count == 1 &&
           result.a2_address == 0x12345678 && result.next_control_cursor == 0x1014);

    input.control_word = (int16_t)0xc000;
    assert(fa18_select_record_streams(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_STREAM_SELECTOR_POST_STREAM_EXTERNAL &&
           result.a1_count == 1 && result.a1_cursor == 0x1000);

    input.control_word = -1;
    assert(fa18_select_record_streams(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_STREAM_SELECTOR_RETURN_ZERO);
    return 0;
}
