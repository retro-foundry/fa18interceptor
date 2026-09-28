#include "renderer_page_setup.h"

#include <string.h>

int fa18_initialize_renderer_page_setup(FA18RendererPageSetup *setup) {
    uint32_t plane_pointers[FA18_COPPER_PAGE_PLANES];

    if (!setup) return -1;
    memset(setup, 0, sizeof *setup);
    for (unsigned index = 0; index < FA18_COPPER_PAGE_PLANES; ++index)
        plane_pointers[index] = index * FA18_COPPER_PAGE_BYTES;
    fa18_five_plane_page_init(&setup->page);
    if (fa18_five_plane_chip_binding_init(
            &setup->chip_binding, setup->chip_bytes, sizeof setup->chip_bytes,
            plane_pointers) != 0)
        return -1;
    for (unsigned index = 0; index < FA18_COPPER_PAGE_PLANES; ++index)
        setup->source[index] = plane_pointers[index];
    for (unsigned index = 0; index < 4; ++index)
        setup->source[5 + index] = setup->source[index];
    setup->table_a[0] = setup->source[3];
    setup->table_a[1] = setup->source[2];
    setup->table_a[2] = setup->source[1];
    setup->table_a[3] = setup->source[0];
    setup->table_a[4] = setup->source[8];
    setup->table_a[5] = setup->source[7];
    setup->table_a[6] = setup->source[6];
    setup->table_a[7] = setup->source[5];
    setup->table_b[0] = setup->source[4];
    setup->table_b[1] = setup->source[3];
    setup->table_b[2] = setup->source[2];
    setup->table_b[3] = setup->source[1];
    setup->table_b[4] = setup->source[0];
    setup->table_b[5] = setup->source[8];
    setup->table_b[6] = setup->source[8];
    setup->table_b[7] = setup->source[7];
    setup->table_b[8] = setup->source[6];
    setup->table_b[9] = setup->source[5];
    return 0;
}
