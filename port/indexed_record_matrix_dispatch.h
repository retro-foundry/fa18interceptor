#ifndef FA18_INDEXED_RECORD_MATRIX_DISPATCH_H
#define FA18_INDEXED_RECORD_MATRIX_DISPATCH_H

#include <stdint.h>

typedef void (*FA18IndexedRecordMatrixDispatch)(void *context);

typedef enum {
    FA18_INDEXED_RECORD_MATRIX_DISPATCH_COMPLETE,
    FA18_INDEXED_RECORD_HEADER_FLAG_CONTINUATION
} FA18IndexedRecordMatrixDispatchRoute;

/* `$C25D86-$C25DA5`: class/header gates and required `$C2D408` dispatch. */
int fa18_run_indexed_record_matrix_dispatch(
    uint8_t record_class,
    uint8_t record_header,
    FA18IndexedRecordMatrixDispatch matrix_dispatch,
    void *context,
    FA18IndexedRecordMatrixDispatchRoute *route);

#endif
