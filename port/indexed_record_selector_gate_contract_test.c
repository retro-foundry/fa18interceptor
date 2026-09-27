#include "indexed_record_selector_gate.h"

#include <assert.h>

static void called(void *context) {
    unsigned *calls = context;
    ++*calls;
}

int main(void) {
    unsigned calls = 0;
    uint8_t selector_called = 0;

    assert(fa18_run_indexed_record_selector_gate(0, 0x1f, 0x10, called, &calls,
                                                  &selector_called) == 0 &&
           calls == 1 && selector_called == 1);
    assert(fa18_run_indexed_record_selector_gate(0x02, 0x10, 0x10, called, &calls,
                                                  &selector_called) == 0 &&
           calls == 1 && selector_called == 0);
    assert(fa18_run_indexed_record_selector_gate(0, 0x20, 0x10, called, &calls,
                                                  &selector_called) == 0 &&
           calls == 1 && selector_called == 0);
    assert(fa18_run_indexed_record_selector_gate(0, 0x10, 0, called, &calls,
                                                  &selector_called) == 0 &&
           calls == 1 && selector_called == 0);
    assert(fa18_run_indexed_record_selector_gate(0, 0x10, 0x10, 0, &calls,
                                                  &selector_called) == -1);
    assert(fa18_run_indexed_record_selector_gate(0, 0x10, 0x10, called, &calls, 0) == -1);
    return 0;
}
