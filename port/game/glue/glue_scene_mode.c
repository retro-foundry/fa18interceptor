/* Register flow through scene stream selection ($C28722). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "scene_dispatch.h"
#include "stages.h"
#include "glue_text.h"

void date_line_registers(int apply); /* glue_batch30.c */
void scene_initialization_registers(void); /* glue_scene_dispatch.c */

static void after_date(void *context) {
    uint8_t raw = rd_u8(MODE_SELECT);
    (void)context;
    date_line_registers(0);
    SET_B(D(0), raw);
    SET_W(D(0), (int8_t)raw);
}

static void selected(gaddr stream, void *context) {
    int16_t variant;
    uint8_t raw = rd_u8(MODE_SELECT);
    (void)context;

    if (raw == 0x7Eu || raw == 0x7Fu) D(0) = 10;
    A(0) = SCENE_DISPATCH_TABLE;
    SET_W(D(0), (uint16_t)(D(0) - 1));
    SET_W(D(0), (uint16_t)(D(0) * 2u));
    SET_W(D(1), (uint16_t)D(0));
    SET_W(D(0), (uint16_t)(D(0) * 2u));
    SET_W(D(0), (uint16_t)(D(0) + D(1)));
    SET_W(D(0), (uint16_t)(D(0) * 2u));
    if (rd_u8(SEQUENCE_FLAG)) SET_W(D(1), rd_u16(SCENE_DISPATCH_VARIANT));
    else {
        SET_W(D(1), rd_u16(SCENE_DISPATCH_BITS));
        SET_W(D(1), (uint16_t)D(1) & 3u);
        if ((uint16_t)D(1)) SET_W(D(1), (uint16_t)(D(1) - 1));
    }
    variant = (int16_t)D(1);
    SET_W(D(1), (uint16_t)(variant * 4));
    SET_W(D(0), (uint16_t)(D(0) + D(1)));
    A(2) = A(0) + (gaddr)(int32_t)rd_s16(A(0) + (gaddr)(int32_t)(int16_t)D(0));
    SET_W(D(0), rd_u16(A(0) + (gaddr)(int32_t)(int16_t)D(0) + 2));

    D(0) = 0;
    SET_B(D(1), raw);
    if (raw != 0x7Eu && raw != 0x7Fu) {
        SET_W(D(1), (int8_t)raw);
        A(0) = rd_u32(MODE_TABLE);
        SET_W(D(0), mode_offset());
    }
    A(2) += (gaddr)(int32_t)rd_s16(A(2) + (gaddr)(int32_t)(int16_t)D(0));
    (void)stream;
}

static void scan(gaddr stream, int accepted, int first, void *context) {
    (void)context;
    A(2) = stream;
    if (!first) {
        SET_W(D(0), rd_u16(stream + 8));
        SET_W(D(0), (uint16_t)D(0) & 0x7Fu);
    }
    if (accepted) scene_initialization_registers();
}

static void finished(gaddr end, void *context) {
    (void)context;
    A(2) = end;
}

static uint32_t swapped(uint32_t v) { return (v << 16) | (v >> 16); }

static void special(void *context) {
    gaddr source = CONTROL_RECORDS;
    gaddr record = CONTROL_RECORDS + 0x800u;
    gaddr viewed = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    uint16_t x, z;
    (void)context;

    A(4) = source;
    A(5) = source;
    SET_W(D(0), rd_u16(STREAM_MODE));
    SET_W(D(0), (uint16_t)(D(0) << 9));
    A(5) += (gaddr)(int32_t)(int16_t)D(0);
    D(0) = 0xFFFFu;
    A(4) += 256;
    A(5) += 256;
    A(0) = SCENE_POINTER_TABLE + 0x3Cu;
    SET_W(D(0), 0x3Cu);
    D(0) = rd_u32(A(0));
    D(1) = rd_u32(A(0) + 4);
    D(2) = rd_u32(A(0) + 8);
    D(3) = rd_u32(A(0) + 12);
    D(4) = rd_u32(A(0) + 16);
    A(4) = SCENE_POINTERS;
    SET_W(D(6), rd_u16(STREAM_MODE));
    SET_W(D(6), (uint16_t)(D(6) * 4u));
    SET_W(D(7), (uint16_t)D(6));
    SET_W(D(6), (uint16_t)(D(6) * 5u));
    D(3) = 0;
    D(4) = 0;
    D(5) = 0x48u;
    world_registers(viewed, VIEW_MATRIX);
    A(1) = record;
    D(0) = swapped(D(0));
    D(2) = swapped(D(2));
    SET_W(D(0), (uint16_t)D(0) >> 6);
    SET_W(D(2), (uint16_t)D(2) >> 6);
    x = (uint16_t)D(0);
    z = (uint16_t)D(2);
    SET_W(D(0), x & 3u);
    SET_W(D(0), (uint16_t)(3u - D(0)));
    SET_W(D(2), z & 3u);
    SET_W(D(2), (uint16_t)(3u - D(2)));
    SET_W(D(2), (uint16_t)(D(2) * 4u));
    SET_W(D(2), (uint16_t)(D(2) + D(0)));
    SET_W(D(0), rd_u16(STREAM_MODE));
}

void scene_mode_run_with_registers(void) {
    SceneDispatchHooks hooks = {after_date, selected, scan, finished, special, 0};
    initialize_scene_from_mode(&hooks);
}

int glue_C28722(void) {
    scene_mode_run_with_registers();
    return glue_return();
}
