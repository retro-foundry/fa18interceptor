#include "ilbm_page_loader.h"

#include <string.h>

enum {
    ILBM_HEADER_BYTES = 12,
    ILBM_CHUNK_HEADER_BYTES = 8,
    ILBM_BMHD_BYTES = 20,
    ILBM_CMAP_BYTES = 96,
    ILBM_SPLSH_WIDTH = 320,
    ILBM_SPLSH_HEIGHT = 200,
    ILBM_SPLSH_PLANES = 5,
    ILBM_SPLSH_ROW_BYTES = 40
};

static uint16_t read_be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static uint32_t read_be32(const uint8_t *bytes) {
    return (uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
           (uint32_t)bytes[2] << 8 | bytes[3];
}

static int same_id(const uint8_t *left, const char right[4]) {
    return memcmp(left, right, 4) == 0;
}

/* ILBM ByteRun1 decodes one plane scanline at a time. */
static int decode_byterun1_row(const uint8_t *body, size_t body_bytes,
                               size_t *input_offset, uint8_t *row) {
    size_t output_offset = 0;

    while (output_offset < ILBM_SPLSH_ROW_BYTES) {
        int8_t control;
        size_t count;
        if (*input_offset >= body_bytes) return -1;
        control = (int8_t)body[(*input_offset)++];
        if (control >= 0) {
            count = (size_t)control + 1u;
            if (count > ILBM_SPLSH_ROW_BYTES - output_offset ||
                count > body_bytes - *input_offset)
                return -1;
            memcpy(row + output_offset, body + *input_offset, count);
            *input_offset += count;
            output_offset += count;
        } else if (control != -128) {
            uint8_t value;
            count = (size_t)(1 - control);
            if (count > ILBM_SPLSH_ROW_BYTES - output_offset ||
                *input_offset >= body_bytes)
                return -1;
            value = body[(*input_offset)++];
            memset(row + output_offset, value, count);
            output_offset += count;
        }
    }
    return 0;
}

int fa18_load_splsh_ilbm_page(FA18FivePlanePage *page,
                              const uint8_t *data, size_t size) {
    const uint8_t *bmhd = 0;
    const uint8_t *cmap = 0;
    const uint8_t *body = 0;
    size_t cmap_bytes = 0, body_bytes = 0, offset = ILBM_HEADER_BYTES;
    size_t body_offset = 0;

    if (!page || !data || size < ILBM_HEADER_BYTES ||
        !same_id(data, "FORM") || !same_id(data + 8, "ILBM") ||
        read_be32(data + 4) != size - 8u)
        return -1;
    while (offset + ILBM_CHUNK_HEADER_BYTES <= size) {
        const uint8_t *chunk = data + offset;
        const uint32_t chunk_bytes = read_be32(chunk + 4);
        size_t next;
        if (chunk_bytes > size - offset - ILBM_CHUNK_HEADER_BYTES)
            return -1;
        next = offset + ILBM_CHUNK_HEADER_BYTES + chunk_bytes;
        if (same_id(chunk, "BMHD")) {
            if (bmhd || chunk_bytes != ILBM_BMHD_BYTES) return -1;
            bmhd = chunk + ILBM_CHUNK_HEADER_BYTES;
        } else if (same_id(chunk, "CMAP")) {
            if (cmap) return -1;
            cmap = chunk + ILBM_CHUNK_HEADER_BYTES;
            cmap_bytes = chunk_bytes;
        } else if (same_id(chunk, "BODY")) {
            if (body) return -1;
            body = chunk + ILBM_CHUNK_HEADER_BYTES;
            body_bytes = chunk_bytes;
        }
        if (next > size || (chunk_bytes & 1u && next == size)) return -1;
        offset = next + (chunk_bytes & 1u);
    }
    if (offset != size || !bmhd || !cmap || !body || cmap_bytes != ILBM_CMAP_BYTES ||
        read_be16(bmhd) != ILBM_SPLSH_WIDTH || read_be16(bmhd + 2) != ILBM_SPLSH_HEIGHT ||
        bmhd[8] != ILBM_SPLSH_PLANES || bmhd[9] != 2u || bmhd[10] != 1u ||
        read_be16(bmhd + 16) != ILBM_SPLSH_WIDTH ||
        read_be16(bmhd + 18) != ILBM_SPLSH_HEIGHT)
        return -1;

    fa18_five_plane_page_init(page);
    for (unsigned colour = 0; colour < 32; ++colour) {
        const uint8_t *rgb = cmap + colour * 3u;
        page->display_state.palette[colour] = (uint16_t)(
            (uint16_t)(rgb[0] >> 4) << 8 | (uint16_t)(rgb[1] >> 4) << 4 |
            (uint16_t)(rgb[2] >> 4));
    }
    for (unsigned row = 0; row < ILBM_SPLSH_HEIGHT; ++row)
        for (unsigned plane = 0; plane < ILBM_SPLSH_PLANES; ++plane)
            if (decode_byterun1_row(body, body_bytes, &body_offset,
                                    page->planes[plane] +
                                    row * ILBM_SPLSH_ROW_BYTES) != 0)
                return -1;
    return body_offset == body_bytes ? 0 : -1;
}
