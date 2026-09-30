/* The scene template stream and its control-record creation ($C28B34). */
#include "scene_dispatch.h"

#include "globals.h"
#include "matrix.h"
#include "control_records.h"

static int16_t record_offset(uint16_t index) {
    return (int16_t)(uint16_t)(index << 9);
}

static uint8_t template_kind(gaddr descriptor) {
    int16_t selector = rd_s16(descriptor);
    uint16_t offset;

    if (selector >= 0) selector = (selector & 0x4000) ? rd_s16(descriptor + 2) : rd_s16(descriptor + 4);
    offset = (uint16_t)selector & 0x0FFFu;
    return rd_u8(descriptor + 6 + offset) & 15u;
}

static int16_t adjustment(uint16_t bits, int shift, uint8_t sign_bit) {
    int16_t value = (int16_t)((bits >> shift) & 3u);
    if (rd_u8(SCENE_DISPATCH_BITS + 1) & sign_bit) value = (int16_t)-value;
    return value;
}

static gaddr geometry(gaddr table, uint16_t offset) {
    return table + (gaddr)(int32_t)rd_s16(table + (gaddr)(int32_t)(int16_t)offset);
}

static void record_components(gaddr record, int16_t x, int16_t z, int16_t dx, int16_t dz, int32_t height) {
    uint32_t x_fixed = (uint32_t)((int64_t)x * 0x400000 + (int64_t)dx * 256);
    uint32_t z_fixed = (uint32_t)((int64_t)z * 0x400000 + (int64_t)dz * 256);

    wr_s16(record + 6, x);
    wr_s16(record + 8, z);
    wr_s16(record + 0x0C, dx);
    wr_s16(record + 0x0E, dz);
    wr_s32(record + 0x10, height);
    wr_u32(record + 0x14, x_fixed);
    wr_u32(record + 0x18, (uint32_t)height << 8);
    wr_u32(record + 0x1C, z_fixed);
}

static void create_record(gaddr record, uint8_t type, uint8_t index, uint8_t kind,
                          uint16_t source_word, uint16_t flags, gaddr parameters, int8_t previous_status) {
    int16_t x = rd_s16(parameters), z = rd_s16(parameters + 2);
    int16_t dx = rd_s16(parameters + 4), dz = rd_s16(parameters + 6);
    int32_t height = rd_s16(parameters + 8);
    int i;

    for (i = 0; i < 41; i++) wr_u32(record + (gaddr)(4 * i), 0);
    wr_u8(record + 0x7D, kind);
    wr_u8(record + 0x62, type);
    wr_u8(record + 0x5E, index);
    wr_u8(record + 0x3A, (uint8_t)((source_word >> 8) & 0x7Fu));
    if (flags & 0x8000u) wr_u8(record + 1, rd_u8(record + 1) | 8u);
    else if ((type & 0xF0u) == 0x10u)
        wr_u8(SCENE_DISPATCH_CREATED, (uint8_t)(rd_u8(SCENE_DISPATCH_CREATED) + 1));
    if (flags & 0x4000u) wr_u8(record + 5, 8);
    if (flags & 0x2000u) wr_u8(record + 1, rd_u8(record + 1) | 1u);

    if (flags & 0x1000u) {
        int16_t add_x, add_z;
        if (rd_u8(SEQUENCE_FLAG)) {
            add_x = rd_s16(SCENE_DISPATCH_SHIFT_X);
            add_z = rd_s16(SCENE_DISPATCH_SHIFT_Z);
        } else {
            uint16_t bits = rd_u16(SCENE_DISPATCH_BITS);
            add_x = adjustment(bits, 1, 0x10u);
            add_z = adjustment(bits, 2, 0x20u);
            wr_s16(SCENE_DISPATCH_SHIFT_X, add_x);
            wr_s16(SCENE_DISPATCH_SHIFT_Z, add_z);
        }
        if (flags & 0x80u) {
            add_x = (int16_t)(add_x >> 1);
            add_z = (int16_t)(add_z >> 1);
        }
        x = (int16_t)(x + add_x);
        z = (int16_t)(z + add_z);
    }

    wr_u8(record + 0x5D, (uint8_t)(previous_status + 1));
    wr_u8(record + 0x7A, 3);
    if ((type & 0xF0u) != 0x20u) wr_u16(record, (uint16_t)(rd_u16(record) | 0x1080u));

    if ((int16_t)source_word < 0) {
        uint16_t selector = source_word & 0x7F00u;
        if (selector) {
            gaddr at = geometry(VIEW_PARAMETER_TABLE, (uint16_t)(selector >> 7));
            for (i = 0; i < 4; i++) wr_u16(record + 0x2C + (gaddr)(2 * i), rd_u16(at + (gaddr)(2 * i)));
            wr_s32(record + 0x34, rd_s16(at + 8));
        }
        wr_u8(record + 0x38, 0xFF);
    } else {
        wr_s16(record + 0x2C, x);
        wr_s16(record + 0x2E, z);
        wr_s16(record + 0x30, dx);
        wr_s16(record + 0x32, dz);
        wr_s32(record + 0x34, height);
        wr_u8(record + 0x38, (uint8_t)((source_word >> 8) | 0x80u));
    }

    if (height) {
        wr_u8(record + 0x7C, (uint8_t)(rd_u8(record + 0x7C) | 0xE0u));
        wr_u16(record + 0x6C, 0x2000);
        wr_u16(record + 0x6E, 0x2000);
    }
    wr_u16(record, (uint16_t)((rd_u16(record) | 0x0140u) & 0x7FFFu));
    wr_u8(record + 0x5F, 0x44);
    wr_u16(record + 0x60, 0x1F4);
    wr_u32(record + 0x72, 0x0061A800u);
    wr_u8(record + 0x63, 0x3D);
    wr_u8(record + 0x71, 0xFF);
    wr_u8(record + 0x64, 0x06);
    wr_u8(record + 0x64, 0x10);
    record_components(record, x, z, dx, dz, height);
    set_record_orientation(record, 0, 0, 0);
}

int dispatch_scene_records(gaddr stream, uint16_t count, int8_t previous_status) {
    uint32_t remaining = (uint32_t)count + 1u;
    int status = previous_status;

    while (remaining--) {
        int8_t incoming_status = previous_status;
        int16_t template_offset = rd_s16(stream);
        uint8_t type = (uint8_t)rd_u16(stream + 2);
        uint16_t source_word = rd_u16(stream + 4);
        uint16_t flags = rd_u16(stream + 8);
        uint8_t index = (uint8_t)(source_word & 0x7Fu);
        gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)record_offset(index);
        gaddr pointers, descriptor, parameters;
        uint8_t kind;
        int i;

        stream += 10;
        status = -1;
        previous_status = -1;
        if ((flags & 0x0F00u) && (uint8_t)((flags >> 8) & 15u) != rd_u8(MODE_SELECT)) continue;
        if (rd_u8(record + 1) & 0x40u) continue;

        pointers = SCENE_POINTERS + (gaddr)(20u * index);
        for (i = 0; i < 5; i++) wr_u32(pointers + (gaddr)(4 * i), rd_u32(SCENE_POINTER_TABLE + (gaddr)(int32_t)template_offset + (gaddr)(4 * i)));
        descriptor = rd_u32(pointers + 4);
        kind = template_kind(descriptor);
        if (rd_s8(SCENE_DISPATCH_ADMITTED) <= rd_s8(SCENE_DISPATCH_CREATED)) continue;
        if (rd_u8(record + 0x62) == 0x15 && rd_u16(record + 6)) continue;
        if (rd_u8(record + 1) & 0x40u) continue;

        parameters = geometry(VIEW_PARAMETER_TABLE, rd_u16(stream - 4));
        create_record(record, type, index, kind, source_word, flags, parameters, incoming_status);
        status = 0;
        previous_status = 0;
    }
    return status;
}

int initialize_scene_record(gaddr stream) {
    int status = dispatch_scene_records(stream, 0, -2);
    if (status >= 0) {
        uint16_t index = rd_u16(stream + 4) & 0x7Fu;
        gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)record_offset(index);
        aim_record_at_view(record, stream);
    }
    return status;
}
