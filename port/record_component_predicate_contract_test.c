#include "record_component_predicate.h"

#include <assert.h>

int main(void) {
    /* `c1fb82_orientation_predicate.md`: the return-bounded frame-601 tuple. */
    FA18RecordComponentPredicateState state = {
        {-818, 0, 478, -1357, -143, 7, -733, -927, 364}, 0, 0, 0, 0,
        {0, 0, 0}, 0, 0
    };
    uint32_t d7;
    uint16_t cursor;
    FA18RecordComponentPredicateRoute route;

    assert(fa18_test_record_component_predicate(&state, 0, 0, &d7, &cursor,
                                                 &route) == 0);
    assert(route == FA18_RECORD_COMPONENT_PREDICATE_ACCEPTED && d7 == 1 && cursor == 0);
    assert(fa18_test_record_component_predicate(&state, UINT32_C(0xabcd0000), 0,
                                                 &d7, &cursor, &route) == 0);
    assert(route == FA18_RECORD_COMPONENT_PREDICATE_ACCEPTED && d7 == 1);
    assert(fa18_test_record_component_predicate(&state, 0x0c00, 0, &d7, &cursor,
                                                 &route) == 0);
    assert(route == FA18_RECORD_COMPONENT_PREDICATE_C1FC3A_EXTERNAL);
    /* Reverse q/r: `$C1FBD4`'s cross product reverses and the final ADD.L
     * takes its BLT arm. */
    state.workspace[3] = -733;
    state.workspace[4] = -927;
    state.workspace[5] = 364;
    state.workspace[6] = -1357;
    state.workspace[7] = -143;
    state.workspace[8] = 7;
    assert(fa18_test_record_component_predicate(&state, UINT32_C(0xabcd0000), 0,
                                                 &d7, &cursor, &route) == 0);
    assert(route == FA18_RECORD_COMPONENT_PREDICATE_REJECTED &&
           d7 == UINT32_C(0xabcd0000));
    return 0;
}
