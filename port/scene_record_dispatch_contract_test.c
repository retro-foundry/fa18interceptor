#include "scene_record_dispatch.h"

#include <assert.h>
#include <string.h>

typedef struct { unsigned calls; } Log;

static int build(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 1 && !input[0] && !input[1] && !input[2]);
    output[0][0] = 4;
    return 0;
}
static int compose(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 2 && !input[0] && !input[1] && input[2] == 0x7070);
    output[2][2] = 5;
    return 0;
}

int main(void) {
    FA18SceneDispatchRecord record;
    memset(&record, 0xa5, sizeof(record));
    Log log = {0};
    const FA18RecordMatrixUpdateOps ops = {build, compose, &log};
    const FA18SceneDispatchCreateInput input = {
        .record_type = 0x10, .record_index = 14, .class_nibble = 5,
        .source_flags = 0xe000, .source_word_1 = 0x2a00,
        .coordinate_x = -2, .coordinate_z = 3,
        .component_x = 4, .component_z = -5, .altitude = 0x12345678,
        .inherited_d7 = -2
    };
    assert(fa18_create_scene_dispatch_record(&record, &input, &ops) == 0);
    assert(log.calls == 2);
    assert(record.bytes[0x7d] == 5 && record.bytes[0x62] == 0x10 &&
           record.bytes[0x5e] == 14 && record.bytes[0x3a] == 0x2a);
    assert(record.bytes[1] == 0xc9);
    assert(record.bytes[5] == 8);
    assert(record.bytes[0x5d] == 0xff);
    assert(record.bytes[0x7a] == 3);
    assert(record.bytes[0] == 0x11 && record.bytes[0x64] == 0x10 &&
           record.bytes[0x7c] == 0xe0 && record.bytes[0x6c] == 0x20 &&
           record.bytes[0x6e] == 0x20);
    assert(record.bytes[0x14] == 0xff && record.bytes[0x15] == 0x80 &&
           record.bytes[0x16] == 0x04 && record.bytes[0x17] == 0x00 &&
           record.bytes[0x1c] == 0x00 && record.bytes[0x1d] == 0xbf &&
           record.bytes[0x1e] == 0xfb && record.bytes[0x1f] == 0x00);
    assert(record.bytes[164] == 0xa5 && record.matrix_update.build_matrix[0][0] == 4);
    return 0;
}
