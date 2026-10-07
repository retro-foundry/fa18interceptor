/* Complete primary/alternate placement traversal, C1CB14-C1CCBA. */
#include "scene_placements.h"
#include "globals.h"
#include "fixed_math.h"

enum {
    ALTERNATE = 0xC45865, OFFSET = 0xC459AA,
    PRIMARY_OFFSET = 0xC459AC, ALTERNATE_OFFSET = 0xC459AE,
    PRIMARY_LIST = 0xC4E9AA, ALTERNATE_LIST = 0xC4F03A,
    DEPTH_TABLE = 0xC41130, COMPARISON = 0xC459B2,
    HEADER_BYTE = 0xC4585B, REFRESH_CLOCK = 0xC458DA,
    SKIP_RELOAD = 0xC458BC, VISIT_CLOCK = 0xC458BD,
    PACKED_POINT = 0xC45B30, DISTANCE_GATE = 0xC45ABA,
    CONTROL_STREAM = 0xC45A36, AUX_STREAM = 0xC45A3A,
    RECORD_BYTES = 24
};

static void observe(const ScenePlacementHooks *hooks, ScenePlacementEvent event) {
    if (hooks->observe) hooks->observe(hooks->context, &event);
}
static gaddr selected_list(void) {
    return rd_u8(ALTERNATE) ? ALTERNATE_LIST : PRIMARY_LIST;
}
static int32_t scaled_distance(int16_t distance, uint16_t shift) {
    return shift >= 32 ? 0 : (int32_t)((uint32_t)(int32_t)distance << shift);
}

static void visit_placement(gaddr record, uint16_t header,
                            const ScenePlacementHooks *hooks) {
    gaddr descriptor = rd_u32(record + 2);
    gaddr routine;
    int16_t point[3];
    unsigned i;
    int32_t cached;
    uint16_t prior_result=header&15u;
    ScenePlacementCall call;
    wr_u8(HEADER_BYTE, (uint8_t)header);
    wr_u16(BOUND_SHIFT, header & 15u);
    observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_DESCRIPTOR,
        .placement=record, .descriptor=descriptor, .header=header});
    if (rd_s16(descriptor) < 0) return;
    routine = rd_u32(descriptor);
    if (routine == 0xC1ED48u && rd_s32(POSITION_BIAS) < -0x380000) return;
    for (i = 0; i < 3; ++i) {
        point[i] = rd_s16(record + 6 + 2 * i);
        wr_s16(BOUND_OFFSET_X + 2 * i, point[i]);
    }
    for (i = 0; i < 3; ++i)
        wr_u32(PACKED_POINT + 4 * i, (uint32_t)(int32_t)point[i] << 8);
    observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_POSITION,
        .placement=record, .x=point[0], .y=point[1], .z=point[2]});
    /* C1ED3C goes straight to descriptor dispatch, bypassing both the cached
     * distance refresh and negative distance/countdown skip gate. */
    if (routine != 0xC1ED3Cu) {
        int refresh;
        cached = scaled_distance(rd_s16(record + 16), header & 15u);
        observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_CACHE,
            .value=(uint32_t)cached});
        refresh = cached < 0x100;
        if (cached >= 0x100 && cached < 0x400) {
            uint16_t clock = rd_u16(REFRESH_CLOCK) & 3u;
            prior_result=clock;
            observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_REFRESH_GATE,
                .header=header, .value=clock});
            refresh = (header & 0x100u) ? clock == 2 : clock == 0;
        }
        if (refresh) {
            int32_t distance;
            /* Reload after the observation: this is the source's MOVEM.W
             * of the shared point, not a cached copy of the list fields. */
            for (i = 0; i < 3; ++i) point[i] = rd_s16(BOUND_OFFSET_X + 2 * i);
            observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_DISTANCE_BEGIN,
                .x=point[0], .y=point[1], .z=point[2]});
            distance = target_distance(point[0], point[1], point[2]);
            observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_DISTANCE_END,
                .value=(uint32_t)distance});
            wr_s16(record + 16, (int16_t)distance);
        }
        wr_u16(DISTANCE_GATE, rd_u16(record + 20));
        if (rd_s16(DISTANCE_GATE) < 0) {
            int countdown = rd_s8(record + 18) - 1;
            wr_u8(record + 18, (uint8_t)countdown);
            /* SUBQ/BGE tests the unwrapped signed subtraction: -128 - 1
             * takes the negative path even though its stored byte is +127. */
            if (countdown >= 0) return;
            wr_u8(record + 18, rd_u8(SKIP_RELOAD));
        }
    }
    wr_u8(record + 19, (uint8_t)(rd_u8(record + 19) - 1));
    wr_u8(VISIT_CLOCK, rd_u8(record + 19));
    call = (ScenePlacementCall){record, descriptor, routine, rd_u32(descriptor + 4),
        header, rd_s16(record + 16), (int16_t)(int8_t)(header >> 8), prior_result};
    wr_s16(MAGNITUDE, call.distance);
    wr_u32(CONTROL_STREAM, rd_u32(descriptor + 8));
    wr_u32(AUX_STREAM, rd_u32(descriptor + 12));
    wr_s16(0xC459B4u, call.kind);
    observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_CALL, .call=&call});
    {
        int32_t result = hooks->consume(hooks->context, &call);
        gaddr base = selected_list();
        int16_t offset = rd_s16(OFFSET);
        if ((int16_t)result <= 0) result = -1;
        wr_s16(base + (gaddr)(int32_t)offset + 20, (int16_t)result);
        observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_RESULT,
            .placement=base, .value=(uint32_t)result, .x=offset});
    }
}

void visit_scene_placements(int alternate, const ScenePlacementHooks *hooks) {
    int32_t negated;
    int16_t comparison;
    wr_u8(ALTERNATE, alternate ? 1 : 0);
    wr_u16(OFFSET, rd_u16(alternate ? ALTERNATE_OFFSET : PRIMARY_OFFSET));
    wr_u8(CELL_CHECKS, 0); wr_u8(POSITION_VALID, 0);
    negated = (int32_t)(0u - rd_u32(PROJECTION_Y));
    if (negated > 0x7fff) comparison = 0x7ffe;
    else comparison = (int16_t)((uint16_t)(int16_t)
        rd_s8(DEPTH_TABLE + (gaddr)(int32_t)((int16_t)negated >> 7)) << 8);
    wr_s16(COMPARISON, comparison);
    observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_SELECT,
        .value=(uint32_t)negated, .x=comparison});
    for (;;) {
        gaddr record = selected_list() + (gaddr)(int32_t)rd_s16(OFFSET);
        uint16_t header = rd_u16(record);
        observe(hooks, (ScenePlacementEvent){.phase=SCENE_PLACEMENT_SCAN,
            .placement=record, .header=header, .x=rd_s16(OFFSET)});
        if (header == 0xffffu) return;
        visit_placement(record, header, hooks);
        wr_u16(OFFSET, (uint16_t)(rd_u16(OFFSET) + RECORD_BYTES));
    }
}
