#include "renderer.h"
#include <assert.h>
#include <string.h>

int main(void) {
    static const int first[] = {145,141,134,127,120,113,107,100,93,86,79,72,
                                65,58,51,44,37,31,24,17,10,3};
    static const int last[] = {208,218,236,255,273,292,310,319,319,319,319,319,
                               319,319,319,319,319,319,319,319,319,319};
    FA18IndexedFrameBuffer frame;
    memset(&frame, 0, sizeof frame);
    static FA18FillSpan spans[51];
    size_t span_count = 0;
    for (int y = 94; y <= 144; ++y) {
        static const int first[] = {145,141,134,127,120,113,107,100,93,86,79,72,
                                    65,58,51,44,37,31,24,17,10,3};
        static const int last[] = {208,218,236,255,273,292,310,319,319,319,319,319,
                                   319,319,319,319,319,319,319,319,319,319};
        spans[span_count++] = (FA18FillSpan){(uint16_t)y,
            (uint16_t)(y <= 115 ? first[y - 94] : 0),
            (uint16_t)(y <= 115 ? last[y - 94] : 319)};
    }
    assert(fa18_apply_fill_spans(&frame, spans, span_count, 2u) == 0);
    for (int y = 0; y < FA18_HEIGHT; ++y) {
        for (int x = 0; x < FA18_WIDTH; ++x) {
            int expected = 0;
            if (y >= 94 && y <= 144) {
                const int row = y - 94;
                expected = x >= (row < 22 ? first[row] : 0) &&
                           x <= (row < 22 ? last[row] : 319);
            }
            assert(((frame.pixels[y * FA18_WIDTH + x] & 2u) != 0u) ==
                   (expected != 0));
        }
    }
    return 0;
}
