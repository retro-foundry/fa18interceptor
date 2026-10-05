#include "command_queue.h"
#include <assert.h>
#include <string.h>

int main(void) {
    FA18CommandInput c = {0};
    FA18FlightCommandState f = {0};
    FA18FlightCommandRecord viewed = {0};
    FA18ViewCommandState v = {0};
    FA18ContextCommandState context = {0};
    FA18CommandQueue q = {0};
    uint8_t data[FA18_COMMAND_QUEUE_NEIGHBORS] = {0};
    uint8_t keys[FA18_COMMAND_KEY_TABLE_SIZE];
    uint32_t event = 0xfeedface;
    unsigned i;
    f.commands = &c; f.viewed = &viewed; v.flight = &f; context.view = &v;
    for (i = 0; i < sizeof keys; ++i) keys[i] = (uint8_t)(i ^ 0x5a);
    data[0x17] = 0x12; data[0x18] = 0x34;
    assert(!fa18_initialize_command_queue(&q, &context, data, sizeof data-1, keys, sizeof keys));
    assert(!q.commands && !context.key_taken);
    assert(fa18_initialize_command_queue(&q, &context, data, sizeof data, keys, sizeof keys));
    assert(c.indexed.throttle == 0x1234 && context.key_taken == &q.taken);
    c.modifier = c.indexed.function_modifier = c.other_modifier = 7;
    assert(fa18_publish_native_command(&q, 0x1234ff23, &event));
    assert(event == 0x12340079 && q.raw[0] == 0x23 && q.translated[0] == 0x79);
    assert(q.taken == 1 && q.count == 1 && q.write_index == 1 && q.translated_index == 0);
    assert(!c.modifier && !c.indexed.function_modifier && !c.other_modifier);
    q.taken = 0; q.count = 0xff; q.write_index = 9;
    assert(fa18_publish_native_command(&q, 0x15, &event));
    assert(!q.count && q.write_index == 10 && q.raw[9] == 0x15);
    q.taken = 0;
    assert(fa18_publish_native_command(&q, 0x16, &event));
    assert(q.write_index == 1 && q.raw[0] == 0x16);

    /* Gated calls still clear modifiers; they do not translate the low word. */
    q.taken = 0; q.count = 10;
    assert(fa18_publish_native_command(&q, 0xabcdff23, &event));
    assert(event == 0xabcdff23 && q.taken == 1 && q.count == 10);
    q.taken = 0; q.count = 0; c.modifier = 1;
    assert(fa18_publish_native_command(&q, 0xabcdff80, &event));
    assert(event == 0xabcdff80 && !q.taken && !q.count && !c.modifier);

    /* Raw signed indices alias the high/low byte of a real native word. */
    q.write_index = (uint8_t)-105;
    assert(fa18_publish_native_command(&q, 0x7f, &event));
    assert(c.indexed.throttle == 0x7f34 && q.write_index == (uint8_t)-104);
    q.taken = 0;
    assert(fa18_publish_native_command(&q, 0x01, &event));
    assert(c.indexed.throttle == 0x7f01);
    q.taken = 0; q.write_index = (uint8_t)-62; /* KEY_TAKEN itself */
    assert(fa18_publish_native_command(&q, 0, &event));
    assert(!q.taken);
    q.translated_index = 14; q.count = 0; q.write_index = 0;
    assert(fa18_publish_native_command(&q, 0x23, &event));
    assert(q.count == 0x79); /* translated store follows the count increment */
    q.taken = 0; q.count = 0; q.translated_index = 12;
    assert(fa18_publish_native_command(&q, 0x23, &event));
    assert(q.write_index == 0x79);

    /* Selection -> actual view action -> publication share live fields. */
    {
        FA18ViewSpanOffsets spans = {{0}};
        CommandRequest request;
        q.taken = 0; q.count = 0; q.write_index = 0;
        q.translated_index = (uint8_t)-68; /* VIEW_MODE */
        c.origin_mode = 0; c.event_counter = 1; c.indexed.mode = 1;
        spans.values[128] = 7;
        c.pending_b = 0x1000;
        assert(fa18_select_pending_command(&c, &request));
        assert(request.action == COMMAND_VIEW_ZERO);
        assert(fa18_apply_view_input_command(&v, &request, &spans, &event));
        assert(!v.mode && v.span_origin == 7);
        assert(fa18_publish_native_command(&q, event, &event));
        assert(v.mode == (uint8_t)event && v.mode == keys[(uint8_t)request.raw_event]);
        q.taken = 0; q.count = 0; q.translated_index = 0;
        q.write_index = (uint8_t)-92; /* ORIGIN_ENABLE */
        assert(fa18_publish_native_command(&q, 0x01, &event));
        assert(c.origin_mode == 1);
        assert(fa18_select_keyboard_command(&c, 0x1a, &request));
        assert(request.origin_mode == 1 && request.action == COMMAND_ZOOM_IN);
    }
    event = 0xfeedface;
    assert(!fa18_publish_native_command(NULL, 0, &event) && event == 0xfeedface);
    assert(!fa18_publish_native_command(&q, 0, NULL));
    return 0;
}
