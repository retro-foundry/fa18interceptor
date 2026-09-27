#include "map_detail_component_route.h"
#include "map_detail_fields.h"

#include <assert.h>

int main(void) {
    assert(fa18_complete_map_detail_component_route(INT32_C(0x12345678),
                                                    INT32_C(0x9abcdef0)) ==
           UINT32_C(0x5678def0));

    const FA18MapDetailFieldsInput input = {
        0, 1, 0, 0, 1, 0x80,
        INT32_C(0x11223344), INT32_C(0x55667788),
        INT32_C(0x00010000), INT32_C(0x00020000)
    };
    FA18MapDetailFieldsResult fields;
    assert(fa18_apply_map_detail_fields(&input, &fields) == 0);
    assert(fa18_complete_map_detail_component_route(fields.coordinate_x,
                                                    fields.coordinate_y) ==
           UINT32_C(0x11325586));
    return 0;
}
