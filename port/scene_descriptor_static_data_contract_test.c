#include "scene_descriptor_static_data.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t descriptor_bytes[0x40] = {0};
    uint8_t entry_bytes[0x20] = {0};
    uint8_t record_bytes[0x30] = {0};
    uint8_t control_bytes[0x40] = {0};
    uint8_t secondary_bytes[1] = {0};
    FA18HunkReloc relocations[] = {
        {0x20, 10}, {0x24, 41}, {0x28, 69}, {0x2c, 45}
    };
    FA18HunkSegment segments[70] = {0};
    FA18Hunks hunks = {segments, 70};
    FA18SceneDescriptorStaticData data;
    FA18SceneDescriptorPointers pointers;
    const uint8_t *bytes;
    size_t size;

    segments[16] = (FA18HunkSegment){FA18_HUNK_CODE, descriptor_bytes,
                                     sizeof descriptor_bytes, relocations, 4};
    segments[10] = (FA18HunkSegment){FA18_HUNK_CODE, entry_bytes, sizeof entry_bytes, 0, 0};
    segments[41] = (FA18HunkSegment){FA18_HUNK_CODE, record_bytes, sizeof record_bytes, 0, 0};
    segments[69] = (FA18HunkSegment){FA18_HUNK_CODE, control_bytes, sizeof control_bytes, 0, 0};
    segments[45] = (FA18HunkSegment){FA18_HUNK_CODE, secondary_bytes, sizeof secondary_bytes, 0, 0};
    descriptor_bytes[0x20 + 3] = 0xdc;
    descriptor_bytes[0x24 + 3] = 0x12;
    descriptor_bytes[0x28 + 3] = 0x0c;

    assert(fa18_load_scene_descriptor_static_data(&hunks, &data) == 0);
    assert(fa18_resolve_scene_descriptor_pointers(&data, 0x00c22068, &pointers) == 0);
    assert(pointers.target_hunk[0] == 10 && pointers.target_offset[0] == 0xdc);
    assert(pointers.target_hunk[1] == 41 && pointers.target_offset[1] == 0x12);
    assert(pointers.target_hunk[2] == 69 && pointers.target_offset[2] == 0x0c);
    assert(pointers.target_hunk[3] == 45 && pointers.target_offset[3] == 0);
    assert(fa18_resolve_scene_descriptor_target(&data, pointers.target_hunk[2],
                                                pointers.target_offset[2],
                                                &bytes, &size) == 0 &&
           bytes == control_bytes + 0x0c && size == sizeof control_bytes - 0x0c);
    assert(fa18_resolve_scene_descriptor_pointers(&data, 0x00c22069, &pointers) == -1);
    assert(fa18_resolve_scene_descriptor_pointers(&data, 0x00c22080, &pointers) == -1);
    assert(fa18_resolve_scene_descriptor_target(&data, 69, sizeof control_bytes,
                                                &bytes, &size) == -1);
    memset(relocations, 0, sizeof relocations);
    assert(fa18_resolve_scene_descriptor_pointers(&data, 0x00c22068, &pointers) == -1);
    return 0;
}
