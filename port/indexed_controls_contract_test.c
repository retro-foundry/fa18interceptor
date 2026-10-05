#include "indexed_controls.h"
#include <assert.h>
#include <string.h>

typedef struct { unsigned calls; uint8_t mode, request; } Child;
static uint32_t changed(void *context, FA18IndexedControls *s) {
    Child *child = context;
    ++child->calls;
    child->mode = s->mode;
    child->request = s->mode_request;
    return 0xabcdef42u;
}

int main(void) {
    FA18IndexedControls s;
    FA18IndexedControlRequest r = {FA18_INDEXED_SELECTION, 0x12345678, 0, 1};
    const int16_t values[] = {0, 0, -1};
    FA18IndexedControlPoses poses = {values, 3};
    Child child = {0};
    uint32_t event = 0;
    memset(&s, 0, sizeof s);
    s.enable_gate = 1;
    assert(fa18_apply_indexed_control(&s, &r, 0, NULL, changed, &child, &event));
    assert(!s.enable_gate && s.enable_selection == 0x10 && event == r.event);
    assert(!child.calls);

    /* F2-derived mode 4 is gated by the original availability byte. */
    s.mode_gate = 1;
    s.modes.status = 1;
    r.selection = 11;
    assert(fa18_apply_indexed_control(&s, &r, 0, NULL, changed, &child, &event));
    assert(!s.mode && !child.calls);
    s.modes.available[3] = 1;
    assert(fa18_apply_indexed_control(&s, &r, 0, NULL, changed, &child, &event));
    assert(s.mode == 4 && child.calls == 1 && child.mode == 4 && event == 0xabcdef42u);

    /* -1 refuses the pose, other negative first words accept it. */
    r.selection = 2;
    s.pose_entry = 9;
    assert(fa18_apply_indexed_control(&s, &r, 0, &poses, changed, &child, &event));
    assert(s.pose_entry == 9 && !s.origin_gate_b);
    r.selection = 1;
    assert(fa18_apply_indexed_control(&s, &r, 0, &poses, changed, &child, &event));
    assert(s.pose_entry == 1 && s.origin_gate_b == 1);
    poses.count = 1;
    event = 0xbad;
    assert(!fa18_apply_indexed_control(&s, &r, 0, &poses, changed, &child, &event));
    assert(s.pose_entry == 1 && event == 0xbad);

    /* F10's $79 level is sign-extended before the original word clamp. */
    r.kind = FA18_INDEXED_FUNCTION_KEY;
    r.event = 0x59;
    s.function_modifier = 0;
    s.recorder_mode = 0;
    s.player_ready = 1;
    assert(fa18_apply_indexed_control(&s, &r, 0, NULL, changed, &child, &event));
    assert(s.function_level == 0x79 && s.throttle == 0x3c0 && s.throttle_companion == 0x3c0);
    r.event = 0x5a;
    assert(fa18_apply_indexed_control(&s, &r, 0, NULL, changed, &child, &event));
    assert(s.function_level == 0x84 && s.throttle == -992);
    s.recorder_mode = 0xfd;
    assert(fa18_apply_indexed_control(&s, &r, -32768, NULL, changed, &child, &event));
    assert(s.function_level == 0x84 && s.throttle == -992 && event == r.event);
    return 0;
}
