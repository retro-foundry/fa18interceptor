#include "ilbm_page_loader.h"

#include <assert.h>
#include <string.h>

enum { BODY_BYTES = 200 * 5 * 2, FILE_BYTES = 12 + 28 + 104 + 8 + BODY_BYTES };

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void put_be32(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 24);
    bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8);
    bytes[3] = (uint8_t)value;
}

static void build_splsh(uint8_t file[FILE_BYTES]) {
    uint8_t *bmhd = file + 20;
    uint8_t *cmap = file + 48;
    uint8_t *body = file + 152;

    memset(file, 0, FILE_BYTES);
    memcpy(file, "FORM", 4);
    put_be32(file + 4, FILE_BYTES - 8);
    memcpy(file + 8, "ILBM", 4);
    memcpy(file + 12, "BMHD", 4);
    put_be32(file + 16, 20);
    put_be16(bmhd, 320);
    put_be16(bmhd + 2, 200);
    bmhd[8] = 5;
    bmhd[9] = 2;
    bmhd[10] = 1;
    put_be16(bmhd + 16, 320);
    put_be16(bmhd + 18, 200);
    memcpy(file + 40, "CMAP", 4);
    put_be32(file + 44, 96);
    for (unsigned colour = 0; colour < 32; ++colour) {
        cmap[colour * 3] = (uint8_t)(colour << 3);
        cmap[colour * 3 + 1] = (uint8_t)(colour << 3);
        cmap[colour * 3 + 2] = (uint8_t)(colour << 3);
    }
    memcpy(file + 144, "BODY", 4);
    put_be32(file + 148, BODY_BYTES);
    for (unsigned row = 0; row < 200; ++row)
        for (unsigned plane = 0; plane < 5; ++plane) {
            const size_t index = ((size_t)row * 5u + plane) * 2u;
            body[index] = 0xd9; /* ByteRun1: repeat 40 bytes. */
            body[index + 1] = (uint8_t)(row == 0 ? plane + 1u : 0u);
        }
}

int main(void) {
    uint8_t file[FILE_BYTES];
    FA18FivePlanePage page;

    build_splsh(file);
    assert(fa18_load_splsh_ilbm_page(&page, file, sizeof file) == 0);
    assert(page.planes[0][0] == 1 && page.planes[4][39] == 5);
    assert(page.planes[3][40] == 0);
    assert(page.display_state.palette[0] == 0 && page.display_state.palette[31] == 0xfff);
    file[10] = 'X';
    assert(fa18_load_splsh_ilbm_page(&page, file, sizeof file) == -1);
    build_splsh(file);
    file[151] = 0xcf;
    assert(fa18_load_splsh_ilbm_page(&page, file, sizeof file) == -1);
    assert(fa18_load_splsh_ilbm_page(0, file, sizeof file) == -1);
    return 0;
}
