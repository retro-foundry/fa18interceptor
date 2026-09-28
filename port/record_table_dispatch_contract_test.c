#include "record_table_dispatch.h"

#include <assert.h>

typedef struct { unsigned calls; } Fixture;
static int dispatch(void *context, uint16_t selector, int16_t *status) {
    Fixture *fixture = context;
    assert(selector == 0x000c); ++fixture->calls; *status = 2; return 0;
}
static int restart(void *context, uint16_t selector, int16_t *status) {
    (void)context; assert(selector == 0x000c); *status = -1; return 0;
}
static int report(void *context, int16_t control) {
    Fixture *fixture = context; assert(control == -1); ++fixture->calls; return 0;
}
int main(void) {
    const uint8_t stream[] = {0x80,0x0c, 0x00,0x0c, 0xff,0xff};
    Fixture fixture = {0};
    FA18RecordTableDispatchInput input = {stream, sizeof stream, 0x2000, 0x2000,
                                           3, 1, dispatch, report, &fixture};
    FA18RecordTableDispatchResult result;
    FA18RecordTableDispatchRoute route;
    assert(fa18_dispatch_record_table_entry(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_TABLE_DISPATCH_CONTINUE_EXTERNAL && fixture.calls == 1 &&
           result.record_status == 3 && result.record_count == 3 && result.selector == 0x000c);
    input.a2_cursor = 0x2002;
    assert(fa18_dispatch_record_table_entry(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_TABLE_DISPATCH_NEXT_CONTROL_EXTERNAL);
    input.a2_cursor = 0x2000; input.dispatch_handler = restart;
    assert(fa18_dispatch_record_table_entry(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_TABLE_DISPATCH_RESTART_EXTERNAL);
    input.a2_cursor = 0x2004; input.dispatch_handler = dispatch;
    assert(fa18_dispatch_record_table_entry(&input, &result, &route) == 0);
    assert(route == FA18_RECORD_TABLE_DISPATCH_CONTROL_EXTERNAL && fixture.calls == 2);
    return 0;
}
