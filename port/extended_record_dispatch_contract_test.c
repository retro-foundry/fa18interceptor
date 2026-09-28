#include "extended_record_dispatch.h"

#include <assert.h>

typedef struct { unsigned transforms, targets; } Fixture;
static int transform(void *context, int16_t count, int16_t offset) {
    Fixture *fixture = context;
    assert(count == 2 && offset == 0x102); ++fixture->transforms; return 0;
}
static int target(void *context, uint16_t selector, int16_t *status) {
    Fixture *fixture = context;
    assert(selector == 0x0034); ++fixture->targets; *status = 1; return 0;
}
int main(void) {
    /* `$C1F94E` positive control, then `$C1F99A` count/offset and `$0034`. */
    const uint8_t stream[] = {0x00,0x02, 0x01,0x02, 0x80,0x34};
    Fixture fixture = {0};
    FA18ExtendedRecordDispatchInput input = {
        stream, sizeof stream, 0x2000, 0x2000, INT16_MAX, 0, 0,
        transform, target, &fixture
    };
    FA18ExtendedRecordDispatchResult result;
    FA18ExtendedRecordDispatchRoute route;
    assert(fa18_dispatch_extended_record(&input, &result, &route) == 0);
    assert(route == FA18_EXTENDED_RECORD_DISPATCH_CONTINUE && fixture.transforms == 1 &&
           fixture.targets == 1 && result.selector == 0x0034 && result.next_a2_cursor == 0x2006);
    return 0;
}
