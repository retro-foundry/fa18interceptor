#include "record_walker_runtime.h"

#include <assert.h>

typedef struct { unsigned triples, dispatches; } Fixture;

static int triple(void *context, int16_t flags, const FA18RecordWalkerTriple triples[3],
                  int *status) {
    Fixture *fixture = context;
    assert(flags == 0 && triples[0].value[0] == 1 && triples[2].value[2] == 9);
    ++fixture->triples; *status = 0; return 0;
}

static int dispatch(void *context, uint16_t selector, uint32_t cursor, int16_t *status) {
    Fixture *fixture = context;
    assert(selector == 0x0034 && cursor == 0x1032);
    ++fixture->dispatches; *status = -1; return 0;
}

int main(void) {
    /* `$C1F6F8` ordinary triple, then `$C1F7A0` A1 selection and `$C1F910`
     * selector `$0034`; the negative target return resumes the terminating A5. */
    const uint8_t stream[] = {
        0x00,0x00, 0x00,0x00, 0x00,0x06, 0x00,0x0c, 0x00,0x0a,
        0x90,0x00, 0x00,0x00, 0x10,0x20, 0xff,0xff,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0x90,0x00, 0x00,0x00, 0x10,0x30,
        0,0, 0,0, 0,0, 0,0, 0,0,
        0x80,0x34
    };
    const uint8_t vertices[] = {
        0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 0,8, 0,9
    };
    Fixture fixture = {0};
    FA18RecordWalkerRuntimeInput input = {
        stream, sizeof stream, 0x1000, 0x1000, 0x1000,
        vertices, sizeof vertices, 8, triple, 0, 0, dispatch, 0, &fixture
    };
    FA18RecordWalkerRuntimeResult result;
    FA18RecordWalkerRuntimeRoute route;
    assert(fa18_run_record_walker_runtime(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_WALKER_RUNTIME_RETURN_ZERO);
    assert(fixture.triples == 1 && fixture.dispatches == 1 &&
           result.dispatched_selectors == 1 && result.cursor == 0x1012);
    return 0;
}
