#include "record_walker_prefix.h"

#include <assert.h>

typedef struct { unsigned calls; } Fixture;

static int triples(void *context, int16_t selector,
                   const FA18RecordWalkerTriple value[3], int *status) {
    Fixture *fixture = context;
    assert(selector == 0 && value[0].value[0] == 1 && value[0].value[2] == 3);
    assert(value[1].value[0] == 4 && value[2].value[2] == 9);
    ++fixture->calls;
    *status = 0;
    return 0;
}

static int triples_nonzero(void *context, int16_t selector,
                            const FA18RecordWalkerTriple value[3], int *status) {
    Fixture *fixture = context;
    assert(selector == 0 && value[0].value[0] == 1 && value[2].value[2] == 9);
    ++fixture->calls;
    *status = 1;
    return 0;
}

static int hex_words(void *context, const int16_t words[6], int *status) {
    Fixture *fixture = context;
    assert(words[0] == 1 && words[5] == 6);
    ++fixture->calls;
    *status = 0;
    return 0;
}

static int other(void *context, int16_t first, int16_t flags, int *status) {
    Fixture *fixture = context;
    assert(first == 7 && (uint16_t)flags == 0x0c00u);
    ++fixture->calls;
    *status = 0;
    return 0;
}

int main(void) {
    const uint8_t vertices[] = {
        0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 0,8, 0,9
    };
    const uint8_t stream[] = {
        0,0, 0,0, 0,6, 0,12, 0,10, 0xff,0xff
    };
    Fixture fixture = {0};
    FA18RecordWalkerPrefixInput input = {
        stream, sizeof stream, 0, 0, vertices, sizeof vertices, 2,
        triples, 0, 0, &fixture
    };
    FA18RecordWalkerPrefixResult result;
    FA18RecordWalkerPrefixRoute route;
    assert(fa18_run_record_walker_prefix(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_WALKER_RETURN_ZERO && fixture.calls == 1 &&
           result.handled_controls == 1 && result.line_emitter_mask == 0xfffff &&
           result.next_line_emitter_mask == 0);

    const uint8_t nonzero_stream[] = {
        0,0, 0,0, 0,6, 0,12, 0,0, 0,12, 0xff,0xff
    };
    fixture.calls = 0;
    input.control_stream = nonzero_stream; input.control_stream_size = sizeof nonzero_stream;
    input.triple_handler = triples_nonzero;
    assert(fa18_run_record_walker_prefix(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_WALKER_RETURN_ZERO && fixture.calls == 1);

    const uint8_t negative[] = {0xff, 0xfe};
    input.control_stream = negative; input.control_stream_size = sizeof negative;
    assert(fa18_run_record_walker_prefix(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_WALKER_NEGATIVE_CONTROL_EXTERNAL);

    const uint8_t hex_stream[] = {
        0,0, 0x20,0, 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,18, 0xff,0xff
    };
    fixture.calls = 0;
    input.control_stream = hex_stream; input.control_stream_size = sizeof hex_stream;
    input.vertex_table = 0; input.vertex_table_size = 0;
    input.triple_handler = 0; input.hex_handler = hex_words; input.other_handler = 0;
    assert(fa18_run_record_walker_prefix(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_WALKER_RETURN_ZERO && fixture.calls == 1);

    const uint8_t other_stream[] = {0,7, 0x0c,0, 0,6, 0xff,0xff};
    fixture.calls = 0;
    input.control_stream = other_stream; input.control_stream_size = sizeof other_stream;
    input.hex_handler = 0; input.other_handler = other;
    assert(fa18_run_record_walker_prefix(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_WALKER_RETURN_ZERO && fixture.calls == 1);

    input.step_budget = 0;
    assert(fa18_run_record_walker_prefix(&input, &result, &route) == -1);
    return 0;
}
