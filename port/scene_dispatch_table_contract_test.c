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
    data[0x40] = 0; data[0x41] = 0x60;
    data[0x60] = 0xff; data[0x61] = 0xfe;
    data[0x62] = 0; data[0x63] = 3;
    data[0x64] = 0; data[0x65] = 4;
    data[0x66] = 0xff; data[0x67] = 0xfb;
    data[0x68] = 0x12; data[0x69] = 0x34;
    FA18SceneDispatchGeometry geometry;
    assert(fa18_scene_dispatch_geometry(&table, 0x40, &geometry) == 0);
    assert(geometry.coordinate_x == -2 && geometry.coordinate_z == 3 &&
           geometry.component_x == 4 && geometry.component_z == -5 &&
           geometry.altitude == 0x1234);
    data[26] = 0; data[27] = 0x80;
    data[0x80] = 0; data[0x81] = 70;
    data[0x82] = 0; data[0x83] = 112;
    data[0x84] = 0x18; data[0x85] = 0;
    data[0x86] = 0x18; data[0x87] = 0;
    data[0x88] = 0; data[0x89] = 0;
    int16_t negative_geometry[5];
    assert(fa18_scene_dispatch_negative_geometry(&table, 26, negative_geometry) == 0);
    assert(negative_geometry[0] == 70 && negative_geometry[1] == 112 &&
           negative_geometry[2] == 6144 && negative_geometry[3] == 6144 &&
           negative_geometry[4] == 0);
    assert(fa18_scene_dispatch_geometry(&table, sizeof data - 1, &geometry) == -1);
    input.mode = 0x7d;
    assert(fa18_select_scene_dispatch_records(&table, &input, &selection) == 1);
    input.mode = 0;
    assert(fa18_select_scene_dispatch_records(&table, &input, &selection) == -1);
    memset(&table, 0, sizeof table);
    assert(fa18_select_scene_dispatch_records(&table, &input, &selection) == -1);
    return 0;
}
