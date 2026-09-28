#include "scene_stream_runtime.h"

#include <assert.h>

static int triples;

static int accept_triples(void *context, int16_t selector,
                          const FA18RecordWalkerTriple values[3],
                          int *source_status) {
    (void)context;
    assert(selector == 0 && values[0].value[0] == 16 &&
           values[1].value[1] == 64 && values[2].value[2] == 144);
    ++triples;
    *source_status = 0;
    return 0;
}

int main(void) {
    /* The selected descriptor starts at +$20.  Its direct branch has three
     * raw triples and the immediately selected control stream names them by
     * `$C48390` offsets, exactly as C1F464/C1F6F8 require. */
    uint8_t bytes[0x80] = {0};
    uint8_t workspace[18] = {0};
    FA18SceneStreamRuntimeInput input = {0};
    FA18SceneStreamRuntimeResult result;
    FA18SceneStreamRuntimeRoute route;

    bytes[0] = 0x40; bytes[1] = 0x20; /* threshold selector with bit 14 */
    bytes[2] = 0x00; bytes[3] = 0x20; /* selected descriptor */
    bytes[0x20 + 6] = 0x00; /* high/low nibbles */
    bytes[0x20 + 7] = 0x00; /* C1F464 direct branch */
    bytes[0x20 + 8] = 3;    /* triple count */
    bytes[0x20 + 10] = 0; bytes[0x20 + 11] = 16;
    bytes[0x20 + 12] = 0; bytes[0x20 + 13] = 32;
    bytes[0x20 + 14] = 0; bytes[0x20 + 15] = 48;
    bytes[0x20 + 16] = 0; bytes[0x20 + 17] = 32;
    bytes[0x20 + 18] = 0; bytes[0x20 + 19] = 64;
    bytes[0x20 + 20] = 0; bytes[0x20 + 21] = 96;
    bytes[0x20 + 22] = 0; bytes[0x20 + 23] = 48;
    bytes[0x20 + 24] = 0; bytes[0x20 + 25] = 96;
    bytes[0x20 + 26] = 0; bytes[0x20 + 27] = 144;
    /* `$C45A36`: one ordinary triple control then its `$FFFF` terminator. */
    bytes[4] = 0; bytes[5] = 0;
    bytes[6] = 0; bytes[7] = 0;
    bytes[8] = 0; bytes[9] = 6;
    bytes[10] = 0; bytes[11] = 12;
    bytes[12] = 0; bytes[13] = 14; /* ordinary-control relative jump */
    bytes[14] = 0xff; bytes[15] = 0xff;

    input.entry = (FA18SceneStreamEntryInput){
        bytes, sizeof bytes, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0
    };
    input.matrix = (FA18TransformMatrix){{256, 0, 0, 0, 256, 0, 0, 0, 256}};
    input.workspace = (FA18SceneStreamTransformedWorkspace){workspace, sizeof workspace};
    input.walker_step_budget = 2;
    input.triple_handler = accept_triples;
    assert(fa18_run_scene_stream_direct_runtime(&input, &result, &route) == 0);
    assert(route == FA18_SCENE_STREAM_RUNTIME_WALKER_COMPLETE && triples == 1 &&
           result.transformed_vertex_count == 3 && workspace[0] == 0 &&
           workspace[1] == 16 && workspace[17] == 144);
    return 0;
}
