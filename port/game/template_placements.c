/* Static template selection and placement refresh, complete $C1D10C.
 * Addresses cite source-owned tables/state, not a physical-world or LOD
 * interpretation. CPU registers and instruction/event timing belong in glue. */
#include "template_placements.h"
#include "control_records.h"
#include "fault.h"
#include "globals.h"
#include "memory.h"
#include "render_state.h"

enum {
    SELECTOR_MAP = 0xC41170, ROW_HELPERS = 0xC41180,
    CURSOR_MAP = 0xC411A0, GROUP_HELPERS = 0xC411B0,
    CONTROL_TRANSLATE = 0xC411F0, CONTROL_DELTAS = 0xC1D764,
    ROOT_ROUTE = 0xC4124E, ROOT_NORMAL = 0xC414B0, ROOT_DERIVED = 0xC42092,
    SELECTOR_A = 0xC45850, SELECTOR_B = 0xC45851, MAP_SELECTOR = 0xC45854,
    DERIVED_ROOT = 0xC45856, ROUTE_FLAG = 0xC45786, ALTERNATE_PACK = 0xC45865,
    RETAIN_CONTEXT = 0xC45866, SELECTOR_GUARD = 0xC45A66,
    TERMS_A = 0xC45948, TERMS_B = 0xC4594C,
    CACHE_A = 0xC4E9AA, CACHE_B = 0xC4F03A, CACHE_APPEND = 0xC4F6CA,
    CONTEXT_A = 0xC459AC, CONTEXT_B = 0xC459AE, CONTEXT_APPEND = 0xC459B0,
    WORKSPACE = 0xC48390, CACHE_COUNT = 0xC4E988, CONTROL_LIST = 0xC4E98A,
    GROUP_LIST = 0xC459CE, GROUP_COUNT = 0xC4585C, GROUP_COUNT_COPY = 0xC4585D,
    EMITTED_COUNT = 0xC4585E, REVERSE_COUNT = 0xC4585F,
    ITEM_INDEX = 0xC459B4, ITEM_FLAGS = 0xC4585A, CELL_PACKET = 0xC459BC,
    WORK_X = 0xC456EE, WORK_Y = 0xC456F2, WORK_Z = 0xC456F6,
    SHIFT_TABLE = 0xC1DF46, CACHE_THRESHOLDS = 0xC1E11E,
    LAST_PLACEMENT = 0xC459C6, DESCRIPTOR_POINTERS = 0xC4D790,
    DESCRIPTOR_PACKETS = 0xC4D7BC, PLACEMENTS_READY = 0xC457A5,
    CELL_BYTES = 0x60, BAND_BYTES = 0x600, PLACEMENT_BYTES = 24
};

typedef struct {
    gaddr root, templates, gates, cache, context;
    gaddr origin_pairs, translation_pairs, row_helpers, group_helpers, translate;
    int16_t column_term, row_term;
    int8_t selector;
} TemplateContext;

static int16_t word_add(int32_t a, int32_t b) { return (int16_t)((uint32_t)a + (uint32_t)b); }
static int32_t long_add(int32_t a, int32_t b) { return (int32_t)((uint32_t)a + (uint32_t)b); }
static int32_t long_sub(int32_t a, int32_t b) { return (int32_t)((uint32_t)a - (uint32_t)b); }
static int32_t long_negate(int32_t a) { return (int32_t)(0u - (uint32_t)a); }
static int32_t long_asr(int32_t a, unsigned count) {
    count &= 63u;
    return count >= 32 ? (a < 0 ? -1 : 0) : a >> count;
}
static int16_t word_asr(int16_t a, unsigned count) {
    count &= 63u;
    return count >= 16 ? (a < 0 ? -1 : 0) : (int16_t)(a >> count);
}
static int16_t word_lsl(uint16_t a, unsigned count) {
    count &= 63u;
    return count >= 16 ? 0 : (int16_t)(a << count);
}
static gaddr at_word(gaddr base, int32_t offset) { return base + (gaddr)(int32_t)(int16_t)offset; }

/* $C1D10C-$C1D272: three source-owned packs share the helper maps. */
static TemplateContext select_context(void) {
    TemplateContext c;
    gaddr terms;
    int16_t mapped;
    wr_u8(DERIVED_ROOT, 0);
    if (rd_u8(CELL_CHECKS)) {
        c.root = rd_u8(ROUTE_FLAG) ? ROOT_ROUTE : ROOT_NORMAL;
        c.cache = CACHE_APPEND; c.context = CONTEXT_APPEND;
        c.templates = TEMPLATE_SELECTOR_Z; c.gates = TEMPLATE_GATES_Z;
        c.origin_pairs = 0xC1D822; c.translation_pairs = 0xC1D876;
        terms = TERMS_B; c.selector = rd_s8(SELECTOR_B);
    } else {
        c.root = ROOT_NORMAL;
        if (!rd_u8(ALTERNATE_PACK) && rd_s32(SELECTOR_GUARD) > -0xA000) {
            c.root = ROOT_DERIVED; wr_u8(DERIVED_ROOT, 1);
        } else if (rd_u8(ROUTE_FLAG)) c.root = ROOT_ROUTE;
        if (rd_u8(ALTERNATE_PACK)) {
            c.templates = TEMPLATE_SELECTOR_Y; c.gates = TEMPLATE_GATES_Y;
            c.cache = CACHE_B; c.context = CONTEXT_B;
        } else {
            c.templates = TEMPLATE_SELECTOR_X; c.gates = TEMPLATE_GATES_X;
            c.cache = CACHE_A; c.context = CONTEXT_A;
        }
        c.origin_pairs = 0xC1D78E; c.translation_pairs = GRID_ADJUST_WORDS;
        terms = TERMS_A; c.selector = rd_s8(SELECTOR_A);
    }
    c.column_term = rd_s16(terms); c.row_term = rd_s16(terms + 2);
    mapped = rd_s8(at_word(SELECTOR_MAP, c.selector));
    c.row_helpers = at_word(ROW_HELPERS, mapped * 8);
    c.group_helpers = at_word(GROUP_HELPERS, mapped * 16);
    c.translate = at_word(CONTROL_TRANSLATE, mapped * 24);
    return c;
}

static gaddr control_cursor(const TemplateContext *c) {
    int16_t offset, extra;
    if (rd_u8(DERIVED_ROOT)) {
        int8_t selector = rd_s8(SELECTOR_A);
        int16_t mapped = rd_s8(at_word(SELECTOR_MAP, selector));
        offset = word_lsl(rd_s8(at_word(CURSOR_MAP, selector)), 5);
        extra = rd_s8(at_word(at_word(GROUP_HELPERS, mapped * 16), rd_s8(SELECTOR_B)));
        offset = word_add(offset, word_add(extra, extra));
    } else {
        offset = rd_s8(at_word(CURSOR_MAP, c->selector));
        if (rd_u8(ROUTE_FLAG)) {
            /* $C1D302 maps the first result through C411A0 a second time. */
            offset = rd_s8(at_word(CURSOR_MAP, offset));
            offset = word_add(offset, offset);
        } else {
            offset = word_lsl(offset, 4);
            extra = rd_s8(at_word(c->row_helpers, rd_s8(MAP_SELECTOR)));
            offset = word_add(offset, word_add(extra, extra));
        }
    }
    offset = rd_s16(at_word(c->root, offset));
    if (offset <= 0) fatal_error(0x42);
    return at_word(c->root, offset);
}

/* ADD.W's signed branch sees the unwrapped sum; CMP sees the saved word. */
static int16_t band_term(int16_t term, int8_t delta, int16_t maximum) {
    int32_t sum = (int32_t)term + delta;
    int16_t value = (int16_t)sum;
    return sum < 0 ? maximum : value > maximum ? 0 : value;
}

static void expand_bands(const TemplateContext *c, gaddr cursor) {
    gaddr markers = WORKSPACE, band = WORKSPACE;
    int remaining = 14, i;
    for (i = 0; i < 14; ++i) fill_column(&markers, 0xFFFF, CELL_BYTES);
    for (;;) {
        int8_t type = rd_s8(cursor++), skip;
        int16_t mapped, maximum, column, row;
        FilingState filing = {0};
        if (type < 0) break;
        if (--remaining < 0) fatal_error(0x0D);
        if (type > 20) fatal_error(0x0C);
        mapped = word_add(rd_s8(at_word(c->translate, type)), rd_s8(at_word(c->translate, type)));
        maximum = rd_u8(CELL_CHECKS) ? 127 : 30;
        column = band_term(c->column_term, rd_s8(at_word(CONTROL_DELTAS, mapped)), maximum);
        row = band_term(c->row_term, rd_s8(at_word(CONTROL_DELTAS, word_add(mapped, 1))), maximum);
        expand_cell_templates(row, column, c->templates, c->gates, band, CONTROL_DELTAS, &filing);
        skip = rd_s8(cursor++); cursor = at_word(cursor, skip);
        band += BAND_BYTES;
    }
    wr_u16(CELL_TIMER, 0x52);
}

/* $C1DE9C/$C1DECE/$C1DEF6: BGE tests the mathematical signed ADD,
 * while the stored/shifted result still wraps to 32 bits. */
static int32_t magnitude_add(int32_t value, int32_t adjustment, unsigned shift) {
    int32_t sum = long_add(value, adjustment);
    if ((int64_t)value + adjustment < 0) sum = long_negate(sum);
    return long_asr(sum, shift);
}

static unsigned placement_shift(uint8_t flags, uint16_t index, int32_t x, int32_t z) {
    gaddr components = GRID_ADJUST_WORDS;
    int32_t y;
    int16_t maximum, other;
    uint16_t shift_index;
    if (flags & 0x10u) {
        components = (flags & 0x40u) ? at_word(WORKSPACE_RECORDS, index * 32)
                                           : at_word(CONTROL_RECORDS, index * 512);
        if (!(flags & 0x40u) && !rd_u8(RETAIN_CONTEXT))
            wr_u8(components + 1, rd_u8(components + 1) & ~4u);
        x = long_add(x, rd_u16(components + 12) & 0x0FFFu);
    }
    maximum = (int16_t)magnitude_add(x, rd_s16(PROJECTION_WORDS), 12);
    if (flags & 0x50u) z = long_add(z, rd_u16(components + 14) & 0x0FFFu);
    other = (int16_t)magnitude_add(z, rd_s16(PROJECTION_WORDS + 4), 12);
    if (other > maximum) maximum = other;
    if (flags & 0x50u) y = magnitude_add(rd_s32(components + 16), rd_s32(PROJECTION_Y), 11);
    else y = long_asr(long_negate(rd_s32(PROJECTION_Y)), 11);
    if ((int16_t)y > maximum) maximum = (int16_t)y;
    shift_index = (uint16_t)maximum >> 1;
    if ((int16_t)shift_index > 0xEF) {
        wr_u16(ERROR_CODE, 0x0F); fault_hook(); shift_index = 0xEF;
    }
    return (uint16_t)(int16_t)rd_s8(at_word(SHIFT_TABLE, shift_index));
}

static void advance_context(const TemplateContext *c) {
    int32_t depth;
    int16_t threshold;
    if (rd_u8(RETAIN_CONTEXT)) return;
    depth = long_asr(long_negate(rd_s32(PROJECTION_Y)), 11);
    if (depth > 3) threshold = 70;
    else {
        uint32_t offset = (uint32_t)depth << 2;
        if (rd_u8(ROUTE_FLAG)) offset = (offset & 0xFFFF0000u) | (uint16_t)(offset + 2);
        threshold = rd_s16(CACHE_THRESHOLDS + offset);
    }
    if (threshold < rd_s16(CACHE_COUNT))
        wr_u16(c->context, rd_u16(c->context) + PLACEMENT_BYTES);
}

static void emit_cell(const TemplateContext *c, gaddr cell, int32_t origin[2],
                      int32_t translation[2], uint8_t *cycle, gaddr *output,
                      uint8_t *ordinal) {
    gaddr end = cell + CELL_BYTES;
    *ordinal = 0;
    wr_u32(LIST_END, end);
    while ((int32_t)cell < (int32_t)end && rd_u8(cell) != 0xFF) {
        uint16_t header = rd_u16(cell), index = header & 255u, descriptor_index = index;
        uint8_t flags = (uint8_t)(header >> 8);
        gaddr descriptor;
        uint16_t payload_x = 0, payload_z = 0;
        int32_t x, z;
        uint32_t tail;
        unsigned shift;
        cell += 2; ++*ordinal;
        wr_u16(ITEM_INDEX, index); wr_u8(ITEM_FLAGS, flags);
        if (index & 0x80u) fatal_error(0x26);
        wr_u16(*output, (uint16_t)((index << 8) | flags));
        if (flags & 0x40u) descriptor_index = rd_u16(at_word(WORKSPACE_RECORDS, index * 32) + 2);
        descriptor = at_word(SCENE_POINTERS, word_lsl(descriptor_index, 4) + word_lsl(descriptor_index, 2));
        wr_u32(*output + 2, descriptor);
        if (!(flags & 0x50u)) { payload_x = rd_u16(cell); cell += 2; }
        if (!rd_u8(CELL_CHECKS)) payload_x = (uint16_t)(payload_x << 2);
        x = long_sub(payload_x, (int32_t)rd_s16(at_word(GRID_ADJUST_WORDS, rd_s8(SELECTOR_A) * 4)) * 4);
        x = long_add(long_add(x, translation[0]), origin[0]);
        wr_s32(WORK_X, x);
        wr_u16(WORK_Y, 0);
        if (!(flags & 0x50u)) { (void)rd_u32(descriptor + 4); payload_z = rd_u16(cell); cell += 2; }
        if (!rd_u8(CELL_CHECKS)) payload_z = (uint16_t)(payload_z << 2);
        z = long_sub(payload_z, (int32_t)rd_s16(at_word(GRID_ADJUST_WORDS, word_add(rd_s8(SELECTOR_A) * 4, 2))) * 4);
        z = long_add(long_add(z, translation[1]), origin[1]);
        wr_s32(WORK_Z, z);
        shift = placement_shift(flags, index, x, z);
        wr_u8(*output + 1, rd_u8(*output + 1) | (uint8_t)shift);
        wr_s16(*output + 6, (int16_t)long_asr(x, shift));
        wr_s16(*output + 8, word_asr(rd_s16(WORK_Y), shift));
        wr_s16(*output + 10, (int16_t)long_asr(z, shift));
        wr_u8(EMITTED_COUNT, rd_u8(EMITTED_COUNT) + 1);
        tail = flags & 0x50u ? 0x80000000u | ((uint32_t)rd_u8(ITEM_INDEX + 1) << 24)
                            : ((uint32_t)*ordinal << 20) | rd_u32(CELL_PACKET);
        wr_u32(*output + 12, tail);
        wr_u16(*output + 16, 0); wr_u8(*output + 18, *cycle);
        *cycle = (uint8_t)(*cycle - 1);
        if ((int8_t)*cycle < 0) *cycle = 3;
        wr_u8(*output + 19, 0); wr_u32(*output + 20, 0);
        wr_u16(CELL_PACKET, (uint16_t)(tail >> 16));
        wr_u16(CACHE_COUNT, rd_u16(CACHE_COUNT) + 1);
        *output += PLACEMENT_BYTES;
        advance_context(c);
        if (rd_s16(CACHE_COUNT) >= 70) *output -= PLACEMENT_BYTES;
    }
}

static gaddr build_placements(const TemplateContext *c, gaddr cursor) {
    gaddr band = WORKSPACE, output = c->cache, group = GROUP_LIST;
    uint8_t cycle = 0;
    if (!rd_u8(RETAIN_CONTEXT)) wr_u16(c->context, 0);
    wr_u16(CACHE_COUNT, 0); wr_u8(GROUP_COUNT, 0);
    wr_u8(EMITTED_COUNT, 0); wr_u8(0xC45867, 0);
    for (;;) {
        int8_t type = rd_s8(cursor++);
        int16_t mapped;
        int32_t origin[2];
        uint8_t count;
        unsigned k;
        if (type < 0) break;
        if (type > 20) fatal_error(0x11);
        mapped = rd_s8(at_word(c->translate, type));
        {
            int8_t dx = rd_s8(at_word(CONTROL_DELTAS, mapped * 2));
            int8_t dz = rd_s8(at_word(CONTROL_DELTAS, word_add(mapped * 2, 1)));
            int16_t x = word_add(rd_s16(TERMS_B), dx), z = word_add(rd_s16(TERMS_B + 2), dz);
            if ((int32_t)rd_s16(TERMS_B) + dx < 0) x = word_add(x, 128);
            if ((int32_t)rd_s16(TERMS_B + 2) + dz < 0) z = word_add(z, 128);
            wr_u32(CELL_PACKET, ((uint16_t)x << 8 & 0xFF00u) | ((uint16_t)z & 255u));
        }
        for (k = 0; k < 2; ++k) {
            origin[k] = rd_s16(at_word(c->origin_pairs, word_add(mapped * 4, k * 2)));
            if (rd_u8(CELL_CHECKS)) origin[k] = long_add(origin[k], rd_s16(at_word(GRID_ADJUST_WORDS, word_add(rd_s8(SELECTOR_A) * 4, k * 2))));
            origin[k] = (int32_t)((uint32_t)origin[k] << 2);
        }
        count = rd_u8(cursor++);
        do {
            int8_t selector = rd_s8(cursor++), selected;
            int32_t translation[2];
            uint8_t ordinal;
            gaddr cell;
            if (selector > 15) fatal_error(0x12);
            selected = rd_s8(at_word(c->group_helpers, selector));
            wr_s16(CELL_PACKET, selected);
            for (k = 0; k < 2; ++k)
                translation[k] = (int32_t)rd_s16(at_word(c->translation_pairs, word_add(selected * 4, k * 2))) * 4;
            cell = at_word(band, selected * CELL_BYTES);
            wr_u32(group, output);
            emit_cell(c, cell, origin, translation, &cycle, &output, &ordinal);
            if (rd_u8(CELL_CHECKS) && !rd_u8(RETAIN_CONTEXT)) {
                if ((int8_t)ordinal <= 1) wr_u32(group, 0xFFFFFFFFu);
                else { group += 4; wr_u16(group, ordinal); group += 2; wr_u32(group, 0xFFFFFFFFu); wr_u8(GROUP_COUNT, rd_u8(GROUP_COUNT) + 1); }
            }
            count = (uint8_t)(count - 1);
        } while ((int8_t)count > 0);
        band += BAND_BYTES;
    }
    wr_u16(output, 0xFFFF); wr_u8(PLACEMENTS_READY, 1);
    wr_u8(GROUP_COUNT_COPY, rd_u8(GROUP_COUNT));
    return output;
}

/* $C1E230-$C1E2A4: translate bounds/vertices and preserve linked sections.
 * Positive links are replaced with the address of the next output section. */
static void copy_descriptor_packet(gaddr source, gaddr destination, uint32_t tail,
                                   int16_t x, int16_t z) {
    uint32_t next;
    uint32_t count, i;
    gaddr link;
    wr_u32(destination, tail); destination += 4;
    wr_u16(destination, rd_u16(source)); source += 2; destination += 2;
    for (i = 0; i < 4; ++i) {
        wr_s16(destination, word_add(rd_s16(source), i < 2 ? x : z));
        source += 2; destination += 2;
    }
    for (;;) {
        wr_u16(destination, rd_u16(source)); source += 2; destination += 2;
        for (i = 0; i < 4; ++i) {
            wr_s16(destination, word_add(rd_s16(source), i < 2 ? x : z));
            source += 2; destination += 2;
        }
        next = rd_u32(source); source += 4;
        link = destination;
        if ((int32_t)next <= 0) wr_u32(destination, next);
        destination += 4;
        count = rd_u16(source); source += 2;
        if (!count) count = 65536; /* DBRA after count-1 still enters once. */
        for (i = 0; i < count; ++i) {
            wr_s16(destination, word_add(rd_s16(source), x));
            wr_u16(destination + 2, rd_u16(source + 2));
            wr_s16(destination + 4, word_add(rd_s16(source + 4), z));
            source += 12; destination += 6;
        }
        if ((int32_t)next <= 0) break;
        wr_u32(link, destination); source = next;
    }
}

static void publish_descriptor_packets(gaddr output) {
    gaddr placement;
    uint8_t accepted = 0;
    if ((int32_t)output < CACHE_APPEND) fatal_error(0x10);
    if (output == CACHE_APPEND) return;
    placement = output - PLACEMENT_BYTES;
    wr_u32(LAST_PLACEMENT, placement); wr_u8(REVERSE_COUNT, 0);
    for (;;) {
        uint16_t header = rd_u16(placement);
        gaddr descriptor = rd_u32(placement + 2);
        if (!(header & 0x50u)) {
            gaddr flags = rd_u32(descriptor + 4), packet = 0;
            if ((int32_t)flags > 0) {
                int16_t index = rd_s16(flags);
                if (index >= 0) index = rd_s16(flags + ((index & 0x4000u) ? 2u : 4u));
                if (index != -1) {
                    int16_t offset = rd_s16(at_word(flags, index & 0x0FFFu));
                    if (offset >= 0) packet = at_word(flags, offset);
                }
            }
            if (packet) {
                int16_t x = word_lsl(rd_u16(placement + 6), header & 15u) & 0x3FFF;
                int16_t z = word_lsl(rd_u16(placement + 10), header & 15u) & 0x3FFF;
                wr_u32(at_word(DESCRIPTOR_POINTERS, accepted * 4), packet);
                /* C1E228/C1E22A already multiply the slot by four before
                 * C1E236 shifts it another six bits: packets stride 256. */
                copy_descriptor_packet(packet, at_word(DESCRIPTOR_PACKETS, accepted * 256), rd_u32(placement + 12), x, z);
            } else wr_u32(at_word(DESCRIPTOR_POINTERS, accepted * 4), 0xFFFFFFFFu);
            if (++accepted >= 11) break;
        }
        wr_u8(REVERSE_COUNT, rd_u8(REVERSE_COUNT) + 1);
        if (rd_s8(REVERSE_COUNT) >= rd_s8(EMITTED_COUNT)) break;
        placement -= PLACEMENT_BYTES;
    }
}

static void publish_control_list(void) {
    gaddr output = CONTROL_LIST, record = CONTROL_RECORDS;
    uint16_t index;
    wr_u16(output, 0xFFFF);
    for (index = 0; index < 16; ++index, record += CONTROL_RECORD_BYTES) {
        if ((rd_u8(record + 1) & 0x44u) == 0x44u) {
            wr_u16(output, index); output += 2; wr_u16(output, 0xFFFF);
        }
    }
}

void refresh_template_placements(void) {
    TemplateContext context = select_context();
    gaddr cursor = control_cursor(&context), output;
    expand_bands(&context, cursor);
    output = build_placements(&context, cursor);
    if (rd_u8(CELL_CHECKS)) publish_descriptor_packets(output);
    if (rd_u8(CELL_CHECKS) && !rd_u8(RETAIN_CONTEXT)) publish_control_list();
}
