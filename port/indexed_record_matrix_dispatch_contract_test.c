#include "indexed_record_matrix_dispatch.h"

#include <assert.h>

static void called(void *context) {
    unsigned *calls = context;
    ++*calls;
}

int main(void) {
    FA18IndexedRecordMatrixDispatchRoute route;
    unsigned calls = 0;

    assert(fa18_run_indexed_record_matrix_dispatch(0x30, 0x80, called, &calls, &route) == 0 &&
           calls == 1 && route == FA18_INDEXED_RECORD_MATRIX_DISPATCH_COMPLETE);
    assert(fa18_run_indexed_record_matrix_dispatch(0x1f, 0x00, called, &calls, &route) == 0 &&
           calls == 2 && route == FA18_INDEXED_RECORD_MATRIX_DISPATCH_COMPLETE);
    assert(fa18_run_indexed_record_matrix_dispatch(0x1f, 0x80, called, &calls, &route) == 0 &&
           calls == 2 && route == FA18_INDEXED_RECORD_HEADER_FLAG_CONTINUATION);
    assert(fa18_run_indexed_record_matrix_dispatch(0x3f, 0xff, called, &calls, &route) == 0 &&
           calls == 3 && route == FA18_INDEXED_RECORD_MATRIX_DISPATCH_COMPLETE);
    assert(fa18_run_indexed_record_matrix_dispatch(0, 0, 0, &calls, &route) == -1);
    assert(fa18_run_indexed_record_matrix_dispatch(0, 0, called, &calls, 0) == -1);
    return 0;
}
