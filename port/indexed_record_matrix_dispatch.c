#include "indexed_record_matrix_dispatch.h"

int fa18_run_indexed_record_matrix_dispatch(
    uint8_t record_class,
    uint8_t record_header,
    FA18IndexedRecordMatrixDispatch matrix_dispatch,
    void *context,
    FA18IndexedRecordMatrixDispatchRoute *route) {
    if (!matrix_dispatch || !route) return -1;
    if ((record_class & 0xf0u) == 0x30u || !(record_header & 0x80u)) {
        matrix_dispatch(context);
        *route = FA18_INDEXED_RECORD_MATRIX_DISPATCH_COMPLETE;
    } else {
        *route = FA18_INDEXED_RECORD_HEADER_FLAG_CONTINUATION;
    }
    return 0;
}
