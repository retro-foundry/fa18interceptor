#include "scene_dispatch_table.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t data[0x300] = {0};
    FA18HunkSegment segments[FA18_SCENE_DISPATCH_TABLE_HUNK + 1] = {{0}};
    segments[FA18_SCENE_DISPATCH_TABLE_HUNK] =
        (FA18HunkSegment){.data = data, .size = sizeof data};
    FA18Hunks hunks = {.segments = segments, .count = FA18_SCENE_DISPATCH_TABLE_HUNK + 1};
    const uint32_t base = FA18_SCENE_DISPATCH_TABLE_OFFSET;
    /* Fade-entry trace: mode $7F, phase low bits 2 -> variant 1, table
     * entry + $70, block + $A0, then list + 8 = `$C2987A`. */
    data[base + 0x70] = 0; data[base + 0x71] = 0xa0;
    data[base + 0x72] = 0; data[base + 0x73] = 6;
    data[base + 0xa0] = 0; data[base + 0xa1] = 8;
    FA18SceneDispatchTable table;
    assert(fa18_load_scene_dispatch_table(&hunks, &table) == 0);
    FA18SceneDispatchSelectionInput input = {
        .mode = 0x7f, .phase_word = 2
    };
    FA18SceneDispatchSelection selection;
    assert(fa18_select_scene_dispatch_records(&table, &input, &selection) == 0);
    assert(selection.records == table.data + 0xa8 && selection.record_type_limit == 6);
    data[base + 0xa8] = 0; data[base + 0xa9] = 0xa0;
    data[base + 0xaa] = 0; data[base + 0xab] = 0x20;
    data[base + 0xac] = 0x8d; data[base + 0xad] = 0x0e;
    data[base + 0xae] = 0; data[base + 0xaf] = 0x40;
    data[base + 0xb0] = 0x80; data[base + 0xb1] = 0;
    data[base + 0xb2] = 0xff; data[base + 0xb3] = 0xff;
    FA18SceneDispatchSourceRecord source;
    assert(fa18_scene_dispatch_source_record(&table, selection.records, 0, &source) == 1);
    assert(source.source_offset == 0x00a0 && source.record_type == 0x20 &&
           source.source_word_1 == 0x8d0e && source.geometry_offset == 0x0040 &&
           source.source_flags == 0x8000);
    assert(fa18_scene_dispatch_source_record(&table, selection.records, 1, &source) == 0);
    assert(fa18_scene_dispatch_source_record(&table, table.data - 1, 0, &source) == -1);
    data[0x40] = 0xff; data[0x41] = 0xfe;
    data[0x42] = 0; data[0x43] = 3;
    data[0x44] = 0; data[0x45] = 4;
    data[0x46] = 0xff; data[0x47] = 0xfb;
    data[0x48] = 0x12; data[0x49] = 0x34;
    data[0x4a] = 0x56; data[0x4b] = 0x78;
    FA18SceneDispatchGeometry geometry;
    assert(fa18_scene_dispatch_geometry(&table, 0x40, &geometry) == 0);
    assert(geometry.coordinate_x == -2 && geometry.coordinate_z == 3 &&
           geometry.component_x == 4 && geometry.component_z == -5 &&
           geometry.altitude == 0x12345678);
    assert(fa18_scene_dispatch_geometry(&table, sizeof data - 11, &geometry) == -1);
    input.mode = 0x7d;
    assert(fa18_select_scene_dispatch_records(&table, &input, &selection) == 1);
    input.mode = 0;
    assert(fa18_select_scene_dispatch_records(&table, &input, &selection) == -1);
    memset(&table, 0, sizeof table);
    assert(fa18_select_scene_dispatch_records(&table, &input, &selection) == -1);
    return 0;
}
