#include "scene_component_magnitude.h"

#include <assert.h>

int main(void) {
    uint16_t words[342];
    for (unsigned index = 0; index != 342; ++index) words[index] = 0x4000;
    const FA18SceneMagnitudeTable table = {words, 342};
    int16_t result;

    assert(fa18_scene_component_magnitude(&table, 16, 0, 0, &result) == 0);
    assert(result == 16);
    assert(fa18_scene_component_magnitude(&table, 3, 4, 0, &result) == 0);
    assert(result == 4);
    assert(fa18_scene_component_magnitude(&table, 0, 0, 0, &result) == 0);
    assert(result == 0);

    words[0] = 0xffff;
    assert(fa18_scene_component_magnitude(&table, 0x7fff, 0, 0, &result) == 0);
    /* Both MULU.W operations retain their wrapped 32-bit result.  The final
     * signed ASR makes this $FFF90006 product negative, so the signed cap is
     * deliberately not taken. */
    assert(result == -28);
    words[0] = 0x4000;

    const FA18SceneMagnitudeTable short_table = {words, 1};
    assert(fa18_scene_component_magnitude(&short_table, 3, 4, 0, &result) == -1);
    assert(fa18_scene_component_magnitude(&table, 0, 0, 0, 0) == -1);
    uint8_t bytes[FA18_SCENE_MAGNITUDE_OFFSET + FA18_SCENE_MAGNITUDE_WORDS * 2u] = {0};
    bytes[FA18_SCENE_MAGNITUDE_OFFSET] = 0x40;
    bytes[FA18_SCENE_MAGNITUDE_OFFSET + 1] = 0x00;
    bytes[FA18_SCENE_MAGNITUDE_OFFSET + 2] = 0x40;
    bytes[FA18_SCENE_MAGNITUDE_OFFSET + 3] = 0x01;
    FA18HunkSegment segments[FA18_SCENE_MAGNITUDE_HUNK + 1] = {{0}};
    segments[FA18_SCENE_MAGNITUDE_HUNK] = (FA18HunkSegment){.kind=FA18_HUNK_CODE,
        .data=bytes,.size=sizeof bytes};
    const FA18Hunks hunks = {segments, FA18_SCENE_MAGNITUDE_HUNK + 1};
    FA18LoadedSceneMagnitudeTable loaded;
    assert(fa18_load_scene_magnitude_table(&hunks, &loaded) == 0);
    assert(loaded.words[0] == 0x4000 && loaded.words[1] == 0x4001);
    PortFieldWindow window; uint16_t word;
    assert(fa18_load_scene_magnitude_window(&hunks,&window)==0);
    assert(window.bytes==bytes && window.origin==FA18_SCENE_MAGNITUDE_OFFSET);
    assert(port_field_window_u16(&window,0,&word) && word==loaded.words[0]);
    assert(port_field_window_u16(&window,2,&word) && word==loaded.words[1]);
    assert(port_field_window_u16(&window,-2,&word) && !word);
    segments[FA18_SCENE_MAGNITUDE_HUNK].size--;
    assert(fa18_load_scene_magnitude_window(&hunks,&window)==-1);
    const FA18Hunks missing = {0, FA18_SCENE_MAGNITUDE_HUNK + 1};
    assert(fa18_load_scene_magnitude_window(&missing,&window)==-1);
    assert(fa18_load_scene_magnitude_table(&missing,&loaded)==-1);
    return 0;
}
