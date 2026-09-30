/* Glue for the face test command $C1FF0A. Its caller reads every register,
 * so the setup's own registers are rebuilt and then the test's glue is run
 * for the registers and flags it leaves; both of its paths only read. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "memory.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

static void call_port(int (*glue)(void), uint32_t return_to) {
    A(7) -= 4;
    wr_u32(A(7), return_to);
    glue();
}

int glue_C1FF0A(void) {
    gaddr stream = A(2);
    uint16_t kind = rd_u16(stream + 6);
    int k;

    test_stream_face(&stream, A(6));
    for (k = 0; k < 3; k++) D(k) = SEXT(rd_u16(A(2) + (gaddr)(2 * k)));
    A(0) = CLIP_INPUT + 4 + 18;
    A(2) += 8;
    A(3) = SEXT(kind);
    SET_W(D(7), kind);
    call_port(glue_C1FB82, 0xC1FF3C);
    A(2) = stream;
    return glue_return();
}
