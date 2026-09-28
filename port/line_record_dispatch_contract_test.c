#include "line_record_dispatch.h"

#include <assert.h>

typedef struct { unsigned calls; int16_t x0, y0, x1, y1; } Capture;

static int emit(void *context, int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    Capture *capture = context;
    ++capture->calls;
    capture->x0 = x0; capture->y0 = y0; capture->x1 = x1; capture->y1 = y1;
    return 0;
}

int main(void) {
    /* Frame-602 `$0034` target input: `$C212B0` A2=`$C3985A`; its first
     * pair is `$C48390+342`/`+348` and emits `(173,68)->(163,68)`. */
    uint8_t vertices[354] = {0};
    const uint8_t stream[] = {0,10, 1,86, (uint8_t)0x81,92};
    FA18ProjectedSegmentPreparationState state = {{5605,1703,5605}, 0};
    Capture capture = {0};
    FA18LineRecordDispatch dispatch = {
        vertices, sizeof vertices, stream, sizeof stream, 0xc3985a,
        &state, emit, &capture, {0}
    };
    int16_t status = 0;

    vertices[342] = 0xff; vertices[343] = 0x33;
    vertices[344] = 0x02; vertices[345] = 0x3a;
    vertices[346] = 0x09; vertices[347] = 0x1e;
    vertices[348] = 0xff; vertices[349] = 0xc5;
    vertices[350] = 0x02; vertices[351] = 0x3a;
    vertices[352] = 0x09; vertices[353] = 0x1e;
    assert(fa18_dispatch_line_record(&dispatch, 0x0034, 0xc3985a, &status) == 0);
    assert(status == 1 && dispatch.last_submission.submitted_pairs == 1 &&
           capture.calls == 1 && capture.x0 == 173 && capture.y0 == 68 &&
           capture.x1 == 163 && capture.y1 == 68);
    assert(fa18_dispatch_line_record(&dispatch, 0x000c, 0xc3985a, &status) == -1);
    return 0;
}
