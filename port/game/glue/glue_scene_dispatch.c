/* Register flow through the scene-record stream ($C28B34). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "scene_dispatch.h"
#include "glue_text.h"

static uint32_t swapped(uint32_t value) { return (value << 16) | (value >> 16); }
void aim_record_registers(int apply); /* glue_batch67.c */

static gaddr geometry_at(uint16_t offset) {
    return VIEW_PARAMETER_TABLE + (gaddr)(int32_t)rd_s16(VIEW_PARAMETER_TABLE + (gaddr)(int32_t)(int16_t)offset);
}

static void dispatch_scene_registers(int parent) {
    uint32_t entries = (uint16_t)D(0) + 1u;
    gaddr cursor = A(2);

    while (entries--) {
        int16_t template_offset = rd_s16(cursor);
        uint16_t type = rd_u16(cursor + 2), source_word = rd_u16(cursor + 4), offset = rd_u16(cursor + 6);
        uint16_t flags = rd_u16(cursor + 8), index = source_word & 0x7Fu;
        gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)(uint16_t)(index << 9);
        gaddr template = SCENE_POINTER_TABLE + (gaddr)(int32_t)template_offset;
        uint8_t admitted = rd_u8(SCENE_DISPATCH_ADMITTED);
        int mode_ok = !(flags & 0x0F00u) || ((flags >> 8) & 15u) == rd_u8(MODE_SELECT);
        int active = (rd_u8(record + 1) & 0x40u) != 0;
        int created = mode_ok && !active && (int8_t)admitted > rd_s8(SCENE_DISPATCH_CREATED)
                   && (rd_u8(record + 0x62) != 0x15 || !rd_u16(record + 6));
        int16_t shift_x = rd_s16(SCENE_DISPATCH_SHIFT_X);
        int sequence = rd_u8(SEQUENCE_FLAG) != 0;
        gaddr parameters = geometry_at(offset);
        uint16_t branch_value = flags;
        uint32_t count = (uint16_t)D(0);
        uint32_t saved_d0;

        if (parent) (void)initialize_scene_record(cursor);
        else (void)dispatch_scene_records(cursor, 0, (int8_t)D(7));
        A(1) = template;
        A(2) = cursor + 4;
        D(0) = swapped(D(0));
        SET_W(D(0), type);
        SET_W(D(3), flags & 0x0F00u);
        if ((uint16_t)D(3)) {
            SET_W(D(3), (uint16_t)D(3) >> 8);
            if ((uint8_t)D(3) != rd_u8(MODE_SELECT)) goto skipped;
        }
        SET_W(D(3), index);
        SET_W(D(1), (uint16_t)D(3));
        A(4) = record;
        SET_W(D(3), (uint16_t)D(3) << 8);
        SET_W(D(3), (uint16_t)(D(3) + D(3)));
        if (active) goto skipped;

        SET_W(D(3), (uint16_t)D(1));
        A(0) = SCENE_POINTERS;
        SET_W(D(3), (uint16_t)(D(3) * 4u));
        SET_W(D(4), (uint16_t)D(3));
        SET_W(D(3), (uint16_t)(D(3) * 4u + D(4)));
        A(0) += (gaddr)(int32_t)(int16_t)D(3);
        D(2) = rd_u32(template);
        D(3) = rd_u32(template + 4);
        D(4) = rd_u32(template + 8);
        D(5) = rd_u32(template + 12);
        D(6) = rd_u32(template + 16);
        A(1) = rd_u32(A(0) + 4);
        SET_W(D(2), rd_u16(A(1)));
        if ((int16_t)D(2) >= 0) {
            SET_W(D(2), (uint16_t)D(2) & 0x4000u);
            SET_W(D(2), rd_u16(A(1) + ((uint16_t)D(2) ? 2u : 4u)));
        }
        SET_W(D(2), (uint16_t)D(2) & 0x0FFFu);
        SET_B(D(4), rd_u8(A(1) + 6 + (uint16_t)D(2)));
        SET_B(D(4), (uint8_t)D(4) & 15u);
        A(0) = record;
        SET_W(D(3), (uint16_t)D(1));
        SET_W(D(3), (uint16_t)D(3) << 8);
        SET_W(D(3), (uint16_t)(D(3) + D(3)));
        SET_B(D(3), admitted);
        if (!created) goto skipped;

        A(4) = parameters + 10;
        if ((int16_t)source_word < 0 && (source_word & 0x7F00u))
            A(4) = geometry_at((uint16_t)((source_word & 0x7F00u) >> 7)) + 8;
        if (flags & 0x1000u) {
            if (sequence) branch_value = (uint16_t)shift_x;
            else branch_value = flags & 0x80u;
        }
        saved_d0 = ((uint32_t)branch_value << 16) | count;
        D(4) = 0;
        D(5) = 0;
        D(6) = 0;
        D(3) = rd_u32(record + 0x1C);
        A(1) = record;
        record_orientation_registers(record, 0, 0, 0);
        D(0) = saved_d0;
        A(0) = record;
        A(2) = cursor + 10;
        D(7) = 0;
        goto next;

skipped:
        A(2) = cursor + 10;
        D(0) = swapped(D(0));
        D(7) = 0xFFFFFFFFu;
next:
        SET_W(D(0), (uint16_t)(D(0) - 1));
        cursor += 10;
    }
}

int glue_C28B34(void) {
    dispatch_scene_registers(0);
    return glue_return();
}

void scene_initialization_registers(void) {
    gaddr original = A(2);
    D(0) = 0;
    D(7) = 0xFFFFFFFEu;
    dispatch_scene_registers(1);
    A(2) = original;
    if ((int16_t)D(7) >= 0) aim_record_registers(0);
}

int glue_C28AFE(void) {
    scene_initialization_registers();
    return glue_return();
}
