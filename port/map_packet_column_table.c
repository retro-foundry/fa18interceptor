#include "map_packet_column_table.h"

static uint16_t add_word(uint16_t left, uint16_t right) {
    return (uint16_t)(left + right);
}

static uint16_t asl_word(uint16_t value, unsigned count) {
    return (uint16_t)(value << count);
}

static uint32_t add_address_word(uint32_t address, uint16_t offset) {
    return (uint32_t)((int64_t)address + (int16_t)offset);
}

int fa18_select_map_packet_column_table(const FA18MapPacketColumnTableInput *input,
                                        FA18MapPacketColumnTableResult *result) {
    if (!input || !result) return -1;
    const uint16_t selector = asl_word((uint16_t)(int16_t)input->table_selector, 4);
    const uint16_t table_a_offset = add_word(selector, selector);
    const uint16_t table_b_offset = add_word(table_a_offset, selector);
    const uint16_t table_c_offset = add_word(table_a_offset, table_a_offset);
    const uint16_t selector_word = (uint16_t)input->metric_selector;
    const uint32_t table_a = add_address_word(UINT32_C(0x00c2a7dc), table_a_offset);
    const uint32_t table_b = add_address_word(UINT32_C(0x00c2a4dc), table_b_offset);
    const uint32_t table_c = add_address_word(UINT32_C(0x00c2a0dc), table_c_offset);

    if (input->metric <= INT32_C(0x3000)) {
        result->stream_address = add_address_word(table_a, asl_word(selector_word, 2));
    } else if (input->metric <= INT32_C(0x4800)) {
        uint16_t offset = asl_word(selector_word, 2);
        offset = add_word(offset, selector_word);
        offset = add_word(offset, selector_word);
        result->stream_address = add_address_word(table_b, offset);
    } else {
        result->stream_address = add_address_word(table_c, asl_word(selector_word, 3));
    }
    return 0;
}
