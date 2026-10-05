#include "flight_command_input.h"
#include <assert.h>
#include <string.h>

typedef struct {
    enum FlightCommandChild calls[8];
    unsigned count;
    FA18FlightCommandRecord *radar_view;
    FA18FlightCommandChildInput last;
} Children;

/* Explicit contracts for unavailable children, used only in this test. */
static int consume(void *context, FA18FlightCommandState *s,
                   enum FlightCommandChild child,
                   const FA18FlightCommandChildInput *input,
                   FlightCommandResult *result) {
    Children *children = context;
    assert(children->count < 8);
    children->calls[children->count++] = child;
    children->last = *input;
    if (fa18_apply_flight_control_child(s, child, input, result)) return 1;
    switch (child) {
    case FLIGHT_THROTTLE_MODE: case FLIGHT_GEAR:
        result->event = input->event;
        result->carried_event_word = input->restore_event_word;
        return 1;
    case FLIGHT_RADAR_RANGE:
        s->viewed = children->radar_view;
        result->event = 0x87654321;
        result->carried_event_word = 0;
        return 1;
    case FLIGHT_CHAFF_SOUND:
        result->event = 0xabcd1234;
        result->carried_event_word = input->restore_event_word;
        return 1;
    default: return 0;
    }
}

int main(void) {
    FA18CommandInput commands;
    FA18FlightCommandRecord records[5];
    FA18FlightCommandState s;
    Children children;
    FA18FlightCommandOps ops = {consume, &children};
    CommandRequest request;
    uint32_t event;
    unsigned i;
    memset(&commands, 0, sizeof commands);
    memset(records, 0, sizeof records);
    memset(&s, 0, sizeof s);
    memset(&children, 0, sizeof children);
    commands.event_counter = commands.indexed.mode_gate = commands.indexed.mode = 1;
    s.commands = &commands;
    s.player = s.viewed = &records[0];
    s.target = &records[4];
    for (i = 0; i < 3; ++i) s.spawn_slots[i] = &records[i+1];

    assert(fa18_select_keyboard_command(&commands, 0x4f, &request));
    assert(request.action == COMMAND_X_RIGHT);
    s.player->stick = 0xff;
    assert(fa18_apply_flight_input_command(&s, &request, 0, &ops, &event));
    assert(s.stick_x == 8 && s.player->stick == 0xff && event == 0x4f);
    s.pause = 1;
    assert(fa18_apply_flight_input_command(&s, &request, 0, &ops, &event));
    assert(s.player->stick == 0xfb);
    assert(fa18_select_keyboard_command(&commands, 0xcf, &request));
    assert(request.action == COMMAND_X_RELEASE);
    assert(fa18_apply_flight_input_command(&s, &request, 0, &ops, &event));
    assert(!s.stick_x && s.player->stick == 0xf3);

    /* Both selector paths and the indexed owner share the throttle state. */
    commands.indexed.function_level = 0x79;
    commands.indexed.throttle = 0x3c0;
    commands.indexed.throttle_companion = -123;
    assert(fa18_select_keyboard_command(&commands, 0x0d, &request));
    assert(request.action == COMMAND_THROTTLE_MODE);
    children.count = 0;
    assert(fa18_apply_flight_input_command(&s, &request, 0, &ops, &event));
    assert(children.count == 2 && children.calls[0] == FLIGHT_THROTTLE_MODE_RELEASE &&
           children.calls[1] == FLIGHT_THROTTLE_MODE);
    assert(!commands.indexed.function_level && !commands.indexed.throttle &&
           !commands.indexed.throttle_companion && s.player->flags == 0x0800);
    commands.pending_a = 0x0100;
    assert(fa18_select_pending_command(&commands, &request));
    assert(request.action == COMMAND_GEAR && !commands.pending_a);
    children.count = 0;
    assert(fa18_apply_flight_input_command(&s, &request, 0, &ops, &event));
    assert(s.emitted_requests == 0x0101 && commands.block_flags == 0x80);

    /* Radar uses the child's latest ordinary view pointer. */
    children.radar_view = &records[1];
    records[1].weapon_radar = 0xab;
    assert(fa18_select_keyboard_command(&commands, 0x13, &request));
    assert(request.action == COMMAND_RADAR_RANGE);
    assert(fa18_apply_flight_input_command(&s, &request, 0, &ops, &event));
    assert(s.viewed == &records[1] && records[1].weapon_radar == 0xa9 &&
           !records[0].weapon_radar && event == 0x87654321);

    /* Signed subtraction overflow takes the empty countermeasure path. */
    s.chaff_count = 0x80;
    s.chaff_timer = 17;
    assert(fa18_select_keyboard_command(&commands, 0x33, &request));
    assert(request.action == COMMAND_CHAFF);
    assert(fa18_apply_flight_input_command(&s, &request, (int16_t)0xbeef, &ops, &event));
    assert(!s.chaff_count && s.chaff_timer == 17 && event == 0xabcdbeef);
    assert(children.last.event == 0x4029 &&
           (uint16_t)children.last.restore_event_word == 0xbeef);

    /* Missing eject/publication owner reports failure after source writes. */
    request.action = COMMAND_EJECT;
    request.modifier = 1;
    event = 0xfeedface;
    assert(!fa18_apply_flight_input_command(&s, &request, 0, &ops, &event));
    assert(s.redraw_e == 8 && event == 0xfeedface);
    assert(!fa18_is_flight_input_command(COMMAND_VIEW_ZERO));
    assert(!fa18_apply_flight_input_command(NULL, &request, 0, &ops, &event));
    return 0;
}
