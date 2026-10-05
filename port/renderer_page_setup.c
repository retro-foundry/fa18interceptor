#include "renderer_page_setup.h"
#include "renderer_page_layout.h"

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
    for (unsigned index = 0; index < 8; ++index)
        setup->table_a[index] = setup->source[fa18_renderer_table_a_order[index]];
    for (unsigned index = 0; index < 10; ++index)
        setup->table_b[index] = setup->source[fa18_renderer_table_b_order[index]];
    return 0;
}
