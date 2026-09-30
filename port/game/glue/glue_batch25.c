/* Glue for the main engine sounds $C17C62, $C17D6E, the tone $C3316A, the
 * edge vertices $C219AE, the buffer clear $C2FD22, the stage blit $C3040C,
 * the grid position $C1EBE0 and the list point $C25876. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"
#include "render_buffers.h"
#include "vertex_tail.h"
#include "view_transform.h"

void play_sound_registers(uint32_t sound, uint32_t channel);                 /* glue_batch20.c */
void play_engine_registers(int32_t period, int32_t volume);                  /* glue_batch23.c */
void slide_engine_registers(int32_t period, int32_t volume, int32_t ticks); /* glue_batch23.c */

#define ARG(n) rd_s32(A(7) + 4 + 4 * (n))

int glue_C17C62(void) {
    int32_t period = ARG(0), volume = ARG(1);
    if (!(rd_u8(SOUND_FLAGS) & 0x01)) {
        play_engine_registers(period, volume >> 2);
    } else {
        int exists = rd_u32(SOUND_VOICES + 4) != 0;
        play_main_engine(period, volume);
        if (exists) play_sound_registers(1, 1);
    }
    return glue_return();
}

int glue_C17D6E(void) {
    int32_t volume = ARG(1);
    slide_engine_registers(ARG(0), (rd_u8(SOUND_FLAGS) & 0x01) ? volume : volume >> 2, ARG(2));
    return glue_return();
}

/* $C3316A: D1 pitch (tone 1). Every register comes back (the MOVEM restore) except
 * D0 = 1, set before the save. */
int glue_C3316A(void) {
    play_tone(1, (int32_t)D(1));
    D(0) = 1;
    return glue_return();
}

/* $C33180, $C3318E, $C33186: the registers come back except D0/D1, set to
 * the variant and pitch before the save. */
int glue_C33180(void) {
    play_tone_2();
    D(0) = 2;
    D(1) = 2;
    return glue_return();
}

int glue_C3318E(void) {
    D(0) = 2;
    D(1) = rd_u8(VOLUME_FADING) ? 2 : 4;
    play_status_tone();
    return glue_return();
}

int glue_C33186(void) {
    if (rd_u8(CONTEXT_SELECT)) return glue_return();
    return glue_C3318E();
}

/* $C219AE: A2 stream. Every register is live after it. */
int glue_C219AE(void) {
    gaddr stream = A(2), bank = WORKSPACES, target;
    int16_t a = rd_s16(stream), b = rd_s16(stream + 2);
    uint32_t pb[3], p6[6];
    int16_t e[3];
    int i, steps;

    target = bank + (gaddr)(int32_t)rd_s16(stream + 4);
    for (i = 0; i < 3; i++) {
        pb[i] = (uint32_t)(int32_t)rd_s16(bank + (gaddr)(int32_t)b + (gaddr)(2 * i));
        e[i] = (int16_t)(pb[i] - (uint32_t)rd_u16(bank + (gaddr)(int32_t)a + (gaddr)(2 * i)));
    }
    for (i = 0; i < 6; i++) p6[i] = (uint32_t)(int32_t)rd_s16(target + 6 + (gaddr)(2 * i));
    A(2) = derive_edge_vertices(stream);

    /* D0-D2 = the edge (halved), high words from the MOVEM.W load of b. */
    D(2) = (pb[2] & 0xFFFF0000u) | (uint16_t)(e[2] >> 1);
    /* D3-D7/A4: +$06/+$0C words (sign-extended) plus the halved edge. */
    D(3) = (p6[0] & 0xFFFF0000u) | (uint16_t)(p6[0] + (uint32_t)(e[0] >> 1));
    D(4) = (p6[1] & 0xFFFF0000u) | (uint16_t)(p6[1] + (uint32_t)(e[1] >> 1));
    D(5) = (p6[2] & 0xFFFF0000u) | (uint16_t)(p6[2] + (uint32_t)(e[2] >> 1));
    D(6) = (p6[3] & 0xFFFF0000u) | (uint16_t)(p6[3] + (uint32_t)(e[0] >> 1));
    D(7) = (p6[4] & 0xFFFF0000u) | (uint16_t)(p6[4] + (uint32_t)(e[1] >> 1));
    A(4) = p6[5] + (uint32_t)(int32_t)(int16_t)(e[2] >> 1);
    A(3) = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    steps = (int8_t)(rd_u8(A(3) + 0x7C) & 0x7F) >> 4;
    D(1) = (pb[1] & 0xFFFF0000u) | (uint16_t)(14 * steps);
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}

/* $C2FD22: the buffer pointers end past their buffers; D0.w = $FFFF. */
void clear_render_buffers_registers(void) {
    int used = rd_u8(FIFTH_BUFFER_USED) != 0;
    A(0) = rd_u32(RENDER_BUFFERS_B + 0) + 4 * RENDER_BUFFER_LONGS;
    A(1) = rd_u32(RENDER_BUFFERS_B + 4) + 4 * RENDER_BUFFER_LONGS;
    A(2) = rd_u32(RENDER_BUFFERS_B + 8) + 4 * RENDER_BUFFER_LONGS;
    A(3) = rd_u32(RENDER_BUFFERS_B + 12) + 4 * RENDER_BUFFER_LONGS;
    A(4) = rd_u32(RENDER_BUFFERS_A + 16) + (used ? 4 * RENDER_BUFFER_LONGS : 0);
    A(5) = rd_u32(RENDER_BUFFERS_B + 16) + 4 * RENDER_BUFFER_LONGS;
    SET_W(D(0), 0xFFFF);
    flags_logic_l(0); /* the last CLR.L */
}

int glue_C2FD22(void) {
    clear_render_buffers();
    clear_render_buffers_registers();
    return glue_return();
}

/* $C3040C: leaves D0.w = the size, D1 the mask source, D2/D3 the plane 1/0
 * addresses, D4.w = $FCA, A0 the custom base, A2 the plane table. */
void mask_between_registers(void);
void mask_between_registers(void) {
    gaddr table = rd_u32(PAGE_PLANE_TABLE);
    uint32_t offset = rd_u32(POLY_PLANE_OFFSET);
    SET_W(D(0), rd_u16(POLY_BLIT_SIZE));
    D(1) = rd_u32(POLY_MASK_SOURCE);
    D(2) = rd_u32(table + 4) + offset;
    D(3) = rd_u32(table) + offset;
    SET_W(D(4), 0x0FCA);
    A(0) = 0xDFF000u;
    A(2) = table;
}

int glue_C3040C(void) {
    blit_mask_between_planes();
    mask_between_registers();
    return glue_return();
}

/* $C1EBE0: A1 record, D1 shift -> D2, D3, D4. */
int glue_C1EBE0(void) {
    int32_t out[3];
    grid_relative_position(A(1), (int)(D(1) & 15), out);
    SET_W(D(1), D(1) & 15);
    D(2) = (uint32_t)out[0];
    D(3) = (uint32_t)out[1];
    D(4) = (uint32_t)out[2];
    /* D5-D7 come back from MOVEM.W, sign-extended. */
    D(5) = (uint32_t)(int32_t)(int16_t)D(5);
    D(6) = (uint32_t)(int32_t)(int16_t)D(6);
    D(7) = (uint32_t)(int32_t)(int16_t)D(7);
    return glue_return();
}

/* $C25876: D2-D4.w point, D1 shift, D7.w tag. Every register is live after
 * it: D2-D4 the second row's products and sum, D5/D6 the first row's
 * x and y products, D7.w the tag over the first sum's high word, A0 past
 * the matrix, A3 the new list end. */
int glue_C25876(void) {
    int16_t x = (int16_t)D(2), y = (int16_t)D(3), z = (int16_t)D(4);
    int count = (int)(D(1) & 63);
    uint16_t tag = (uint16_t)D(7);
    int32_t first, second;
    append_list_point(x, y, z, (int)D(1), tag);
    first = ((int32_t)x * rd_s16(LIST_MATRIX) + (int32_t)y * rd_s16(LIST_MATRIX + 2) + (int32_t)z * rd_s16(LIST_MATRIX + 4)) >> 8;
    second = ((int32_t)x * rd_s16(LIST_MATRIX + 12) + (int32_t)y * rd_s16(LIST_MATRIX + 14) + (int32_t)z * rd_s16(LIST_MATRIX + 16)) >> 8;
    first = count >= 32 ? 0 : (int32_t)((uint32_t)first << count);
    second = count >= 32 ? 0 : (int32_t)((uint32_t)second << count);
    D(5) = (uint32_t)((int32_t)x * rd_s16(LIST_MATRIX));
    D(6) = (uint32_t)((int32_t)y * rd_s16(LIST_MATRIX + 2));
    D(7) = ((uint32_t)first & 0xFFFF0000u) | tag;
    D(2) = (uint32_t)((int32_t)x * rd_s16(LIST_MATRIX + 12));
    D(3) = (uint32_t)((int32_t)y * rd_s16(LIST_MATRIX + 14));
    D(4) = (uint32_t)second;
    A(0) = LIST_MATRIX + 18;
    A(3) = rd_u32(LIST_WRITE);
    return glue_return();
}

/* $C2F582: D0-D7, A4, A5 = 0 (MOVEQ/MOVE.L's flags: Z), A0-A3 = page 1's
 * plane pointers. */
int glue_C2F582(void) {
    int i;
    clear_page_plane_tops();
    for (i = 0; i < 8; i++) D(i) = 0;
    for (i = 0; i < 4; i++) A(i) = rd_u32(PAGE0_PLANE_TABLE + 16 + (gaddr)(4 * i));
    A(4) = 0;
    A(5) = 0;
    flags_logic_l(0);
    return glue_return();
}
