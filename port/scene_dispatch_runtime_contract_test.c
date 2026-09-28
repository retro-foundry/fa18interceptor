#include "scene_dispatch_runtime.h"
#include "coordinate_update_positive_pair.h"

#include <assert.h>
#include <string.h>

static int build(void *context, const int16_t input[3], int16_t output[3][3]) {
    (void)context;
    assert(!input[0] && !input[2] && (input[1] == 0 || input[1] == 0x6fb8));
    output[0][0] = 0x4000;
    output[1][1] = 0x4000;
    output[2][2] = 0x4000;
    return 0;
}

static int compose(void *context, const int16_t input[3], int16_t output[3][3]) {
    (void)context;
    assert(input[2] == 0 || input[2] == 0x7060);
    output[0][0] = 0x4000;
    output[1][1] = 0x4000;
    output[2][2] = 0x4000;
    return 0;
}

int main(void) {
    uint8_t template_bytes[0x200] = {0};
    uint8_t dispatch_bytes[0x400] = {0};
    uint8_t descriptor_bytes[0x100] = {0};
    uint8_t coordinate_bytes[64] = {0};
    FA18HunkReloc relocs[5];
    FA18HunkSegment segments[53] = {{0}};
    FA18Hunks hunks = {segments, 53};
    FA18SceneDispatchTable table;
    FA18SceneDispatchRuntime runtime = {0};
    FA18SceneNegativePoseRecord record;
    FA18SceneNegativePoseDescriptor descriptor;
    const FA18RecordMatrixUpdateOps matrix_ops = {build, compose, 0};
    const uint32_t base = FA18_SCENE_DISPATCH_TABLE_OFFSET;
    const FA18CoordinateAngleTable coordinate_table = {coordinate_bytes, sizeof coordinate_bytes};

    for (unsigned i = 0; i != 5; ++i)
        relocs[i] = (FA18HunkReloc){0xa0u + i * 4u, 52};
    segments[16] = (FA18HunkSegment){FA18_HUNK_CODE, template_bytes,
                                      sizeof template_bytes, relocs, 5};
    segments[27] = (FA18HunkSegment){.data = dispatch_bytes,
                                      .size = sizeof dispatch_bytes};
    segments[52] = (FA18HunkSegment){.data = descriptor_bytes,
                                      .size = sizeof descriptor_bytes};
    /* Mode $7F uses the mode-10 entry, then list-block + list offset. */
    dispatch_bytes[base + 0x6c + 1] = 0xa0;
    dispatch_bytes[base + 0x6c + 3] = 6;
    dispatch_bytes[base + 0xa0 + 1] = 8;
    dispatch_bytes[base + 0xa8 + 1] = 0xa0;
    dispatch_bytes[base + 0xaa + 1] = 0x20;
    dispatch_bytes[base + 0xac] = 0x8d;
    dispatch_bytes[base + 0xad] = 0x0e;
    dispatch_bytes[base + 0xae + 1] = 0x40;
    dispatch_bytes[base + 0xb0] = 0x80;
    dispatch_bytes[base + 0xb2] = 0xff;
    dispatch_bytes[base + 0xb3] = 0xff;
    dispatch_bytes[0x40] = 0; dispatch_bytes[0x41] = 0x60;
    dispatch_bytes[0x60] = 0; dispatch_bytes[0x61] = 0x44;
    dispatch_bytes[0x62] = 0; dispatch_bytes[0x63] = 0x44;
    dispatch_bytes[0x64] = 0x18; dispatch_bytes[0x65] = 0;
    dispatch_bytes[0x66] = 0x18; dispatch_bytes[0x67] = 0;
    dispatch_bytes[26] = 0; dispatch_bytes[27] = 0x80;
    dispatch_bytes[0x80] = 0; dispatch_bytes[0x81] = 70;
    dispatch_bytes[0x82] = 0; dispatch_bytes[0x83] = 112;
    dispatch_bytes[0x84] = 0x18; dispatch_bytes[0x85] = 0;
    dispatch_bytes[0x86] = 0x18; dispatch_bytes[0x87] = 0;
    coordinate_bytes[22] = 0; coordinate_bytes[23] = 25;
    template_bytes[0xa0 + 4] = 0;
    template_bytes[0xa0 + 5] = 0;
    template_bytes[0xa0 + 6] = 0;
    template_bytes[0xa0 + 7] = 0;
    template_bytes[0xa0 + 16] = 0;
    template_bytes[0xa0 + 17] = 0;
    template_bytes[0xa0 + 18] = 0;
    template_bytes[0xa0 + 19] = 0x80;
    descriptor_bytes[0] = 0x80; descriptor_bytes[1] = 3;
    descriptor_bytes[9] = 5;
    descriptor_bytes[0x82] = 0x80;
    descriptor_bytes[0x85] = 0x70;

    assert(fa18_load_scene_dispatch_table(&hunks, &table) == 0);
    { int16_t geometry[5], output[2];
      assert(fa18_scene_dispatch_negative_geometry(&table, 26, geometry) == 0);
      assert(geometry[0] == 70 && geometry[1] == 112 && geometry[2] == 6144 &&
             geometry[3] == 6144 && geometry[4] == 0);
      assert(fa18_update_coordinate_positive_pair(&coordinate_table,
             &(FA18SceneCoordinateUpdateInput){0, 0, 0x00800000, 0,
                                                 0x0b000000, -1}, output) == 0);
      assert(output[1] == 0x6fb8); }
    assert(fa18_initialize_scene_dispatch_runtime(
               &hunks, &table, &(FA18SceneDispatchSelectionInput){.mode = 0x7f},
               -2, &coordinate_table, &matrix_ops, &runtime) == 0);
    assert(runtime.record[14].bytes[0] == 0x01);
    assert(runtime.record[14].bytes[1] == 0x48);
    assert(runtime.record[14].bytes[0x62] == 0x20);
    assert(runtime.record[14].bytes[0x7d] == 5);
    assert(fa18_scene_dispatch_runtime_resolve_negative_record(&runtime, 14, &record) == 0);
    assert(record.flags_byte_01 == 0x48);
    assert(record.base.value[0] == 0x11180000);
    assert(record.base.value[1] == 0);
    assert(record.base.value[2] == 0x11180000);
    assert(record.matrix.value[0][0] == 0x4000);
    assert(record.angles[1] == 0x6fb8);
    assert(fa18_scene_dispatch_runtime_resolve_negative_descriptor(
               &runtime, 14, &descriptor) == 0 &&
           (uint32_t)descriptor.value_02 == 0x80000070u);
    assert(fa18_initialize_scene_dispatch_runtime(
               &hunks, &table, &(FA18SceneDispatchSelectionInput){.mode = 0x7d},
               -2, &coordinate_table, &matrix_ops, &runtime) == 1);
    return 0;
}
