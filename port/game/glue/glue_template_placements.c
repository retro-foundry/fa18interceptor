/* Complete C1D10C whole-call adapter. Domain observations carry semantic
 * coordinates, levels, bounds and cursors; all register/CCR effects stay
 * here. No original instructions or repeated child writes are executed. */
#include "glue.h"
#include "globals.h"
#include "template_placements.h"
#include "glue_cell_template_outputs.h"

typedef struct {
    CellTemplateOutputs expansion;
    uint16_t count_word;
} TemplateAdapter;

static uint32_t arithmetic_right(uint32_t value, unsigned count) {
    count &= 63u;
    return count >= 32 ? ((int32_t)value < 0 ? 0xffffffffu : 0)
                       : (uint32_t)((int32_t)value >> count);
}
static uint16_t shift_word(uint16_t value, unsigned count) {
    count &= 63u;
    return count >= 16 ? 0 : (uint16_t)((uint32_t)value << count);
}

static void template_outputs(void *opaque, const TemplatePlacementEvent *e) {
    TemplateAdapter *adapter = opaque;
    unsigned shift;
    switch (e->phase) {
    case TEMPLATE_HELPERS:
        SET_W(D(0), e->word * 24); SET_W(D(1), e->word * 8);
        A(1) = e->table; break;
    case TEMPLATE_MARKERS:
        SET_W(D(0), 0xffff); SET_W(D(1), 96); D(2) = 0xffff;
        A(1) = e->cursor; break;
    case TEMPLATE_CURSOR:
        SET_W(D(0), e->word);
        if (e->flags) SET_W(D(1), e->extra * 2);
        if (e->table) A(1) = e->table;
        A(0) = e->cursor; A(3) = 0xc48390u; D(4) = 14; break;
    case TEMPLATE_BAND_TYPE:
        SET_B(D(0), e->type); A(0) = e->cursor; break;
    case TEMPLATE_EXPAND_BEGIN:
        SET_W(D(4), D(4) - 1); SET_W(D(3), e->word);
        SET_W(D(2), e->extra); SET_W(D(1), e->y); SET_W(D(0), e->x);
        A(2) = e->cursor; A(1) = e->cell;
        cell_template_outputs_begin((int16_t)e->x, (int16_t)e->y,
                                    e->table, e->other, e->cell, e->cursor,
                                    &adapter->expansion);
        break;
    case TEMPLATE_EXPAND_END: {
        FilingState filing = *e->filing;
        if (!filing.level_offset_valid)
            filing.level_offset = (int16_t)(adapter->expansion.d1 >> 16);
        cell_template_outputs_end(&adapter->expansion, &filing, rd_u8(CELL_CHECKS) != 0);
        break;
    }
    case TEMPLATE_BAND_NEXT:
        SET_W(D(0), e->word); A(0) = e->cursor; A(3) = e->cell; break;
    case TEMPLATE_BUILD_BEGIN:
        A(0) = e->cursor; A(2) = e->output;
        A(5) = 0xc459ceu; A(4) = 0xc48390u; break;
    case TEMPLATE_BUILD_TYPE:
        SET_B(D(0), e->type); A(0) = e->cursor; break;
    case TEMPLATE_ORIGIN:
        SET_W(D(0), e->word * 4); SET_W(D(1), e->word * 2);
        if (rd_u8(CELL_CHECKS)) {
            int16_t offset = (int16_t)(rd_s8(0xc45850u) * 4);
            SET_W(D(6), offset);
            D(0) = (uint32_t)(int32_t)rd_s16(GRID_ADJUST_WORDS + (uint32_t)(int32_t)offset);
            D(1) = (uint32_t)(int32_t)rd_s16(GRID_ADJUST_WORDS + (uint32_t)(int32_t)(int16_t)(offset + 2));
        }
        D(2) = (uint32_t)e->x; D(3) = (uint32_t)e->y; A(1) = e->table; break;
    case TEMPLATE_COUNT:
        SET_B(D(0), e->count); A(0) = e->cursor; break;
    case TEMPLATE_CELL:
        SET_W(D(6), e->word * 32); D(1) = 0;
        D(4) = (uint32_t)e->x; D(5) = (uint32_t)e->y;
        A(0) = e->cursor; A(1) = e->table; A(3) = e->cell; A(5) = e->group; break;
    case TEMPLATE_SHIFT:
        adapter->count_word = (uint16_t)D(0);
        D(6) = (uint32_t)e->y; SET_W(D(6), (int16_t)e->value);
        A(1) = 0xc1df46u; break;
    case TEMPLATE_PLACEMENT:
        D(0) = arithmetic_right((uint32_t)e->x, (uint16_t)e->word);
        SET_W(D(0), adapter->count_word);
        D(7) = e->value; SET_B(D(6), e->cycle); SET_B(D(1), e->ordinal);
        A(3) = e->cell; A(2) = e->output; break;
    case TEMPLATE_CONTEXT:
        if (e->output) A(2) = e->output;
        else { D(6) = e->value; SET_W(D(6), e->word); A(1) = e->table; }
        break;
    case TEMPLATE_CELL_END:
        SET_B(D(0), e->count); A(5) = e->group; break;
    case TEMPLATE_BUILD_NEXT: A(4) = e->cell; break;
    case TEMPLATE_BUILD_END: A(2) = e->output; break;
    case TEMPLATE_REVERSE_BEGIN:
        D(7) = 0xc4f6cau;
        if (e->output != 0xc4f6cau) { A(2) = e->output - 24; SET_B(D(0), 0); }
        break;
    case TEMPLATE_REVERSE_RECORD:
        SET_W(D(7), e->word); A(1) = e->table;
        D(2) = (uint32_t)e->x; D(3) = (uint32_t)e->y; D(4) = (uint32_t)e->z;
        D(6) = e->value; A(2) = e->cell + 16; break;
    case TEMPLATE_DESCRIPTOR_FLAGS:
        D(1) = e->value; if ((int32_t)e->value > 0) A(0) = e->value; break;
    case TEMPLATE_DESCRIPTOR_WORD: case TEMPLATE_DESCRIPTOR_OFFSET:
        SET_W(D(1), e->word); break;
    case TEMPLATE_PACKET_BEGIN:
        shift = (uint16_t)e->word & 15u; SET_W(D(7), shift);
        SET_W(D(2), shift_word((uint16_t)D(2), shift) & 0x3fffu);
        SET_W(D(3), shift_word((uint16_t)D(3), shift));
        SET_W(D(4), shift_word((uint16_t)D(4), shift) & 0x3fffu);
        SET_W(D(0), (int16_t)(int8_t)D(0)); SET_W(D(1), e->count * 256);
        A(0) = e->cursor; A(1) = 0xc4d7bcu + e->count * 256u; break;
    case TEMPLATE_PACKET_SECTION:
        D(1) = e->value; SET_W(D(6), e->word); SET_W(D(7), 0xffff);
        A(0) = e->cursor; A(1) = e->output; break;
    case TEMPLATE_PACKET_LINK:
        D(7) = e->other; A(0) = e->cursor; A(1) = e->output; break;
    case TEMPLATE_PACKET_MISSING:
        SET_W(D(0), (int16_t)(int8_t)D(0)); SET_W(D(1), e->count * 4);
        A(1) = 0xc4d790u; break;
    case TEMPLATE_PACKET_ACCEPT: SET_B(D(0), e->count); break;
    case TEMPLATE_REVERSE_VISIT: SET_B(D(7), e->count); break;
    case TEMPLATE_CONTROL_END:
        SET_W(D(0), 16); A(0) = e->cursor; A(2) = CONTROL_RECORDS + 16 * CONTROL_RECORD_BYTES; break;
    case TEMPLATE_FINISH:
        flags_logic_b(rd_u8(CELL_CHECKS) ? rd_u8(0xc45866u) : 0); break;
    }
}

int glue_C1D10C(void) {
    TemplateAdapter adapter = {0};
    TemplatePlacementObserver observer = {template_outputs, &adapter};
    refresh_template_placements_observed(&observer);
    return glue_return();
}
