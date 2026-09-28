#include "renderer_page_setup.h"

#include <assert.h>

int main(void) {
    FA18RendererPageSetup setup;
    const uint32_t planes[5] = {0, 8000, 16000, 24000, 32000};
    const uint32_t table_a[8] = {24000, 16000, 8000, 0, 24000, 16000, 8000, 0};
    const uint32_t table_b[10] = {32000, 24000, 16000, 8000, 0,
                                  24000, 24000, 16000, 8000, 0};

    assert(fa18_initialize_renderer_page_setup(&setup) == 0);
    for (unsigned index = 0; index < 5; ++index) {
        assert(setup.chip_binding.plane_pointers[index] == planes[index]);
        assert(setup.source[index] == planes[index]);
    }
    assert(setup.source[5] == 0 && setup.source[6] == 8000 &&
           setup.source[7] == 16000 && setup.source[8] == 24000);
    for (unsigned index = 0; index < 8; ++index)
        assert(setup.table_a[index] == table_a[index]);
    for (unsigned index = 0; index < 10; ++index)
        assert(setup.table_b[index] == table_b[index]);
    assert(fa18_initialize_renderer_page_setup(0) == -1);
    return 0;
}
