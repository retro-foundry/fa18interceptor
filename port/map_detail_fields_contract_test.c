#include "map_detail_fields.h"

#include <assert.h>

int main(void) {
    FA18MapDetailFieldsInput input = {
        0, 1, 0, 0, 1, 0x80,
        INT32_C(0x11223344), INT32_C(0x55667788),
        INT32_C(0x00010000), INT32_C(0x00020000)
    };
    FA18MapDetailFieldsResult result;
    assert(fa18_apply_map_detail_fields(&input, &result) == 0);
    assert(result.visible == 1 && result.coordinate_x == INT32_C(0x33441132) &&
           result.coordinate_y == INT32_C(0x77885586));

    input.alternate_layout = 1;
    assert(fa18_apply_map_detail_fields(&input, &result) == 0);
    assert(result.coordinate_x == INT32_C(0x34411233) &&
           result.coordinate_y == INT32_C(0x78855687));

    input.force_visible = 0;
    input.visibility_gate = 0;
    assert(fa18_apply_map_detail_fields(&input, &result) == 0 && !result.visible);

    input.visibility_gate = 1;
    input.detail_metric = 0;
    input.coordinate_x = 0;
    input.coordinate_y = 0;
    input.offset_x = 0;
    input.offset_y = 0;
    assert(fa18_apply_map_detail_fields(&input, &result) == 0 && !result.visible);
    input.coordinate_x = INT32_C(0x01000000);
    assert(fa18_apply_map_detail_fields(&input, &result) == 0 && result.visible);

    input.detail_metric = INT32_C(-0x100);
    assert(fa18_apply_map_detail_fields(&input, &result) == -1);
    return 0;
}
