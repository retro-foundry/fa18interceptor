#include "context_workspace_flags.h"

static int has_lanes(size_t size, size_t stride, size_t count) {
    return count != 0 && stride <= SIZE_MAX / (count - 1u) &&
           size >= (count - 1u) * stride + FA18_CONTEXT_WORKSPACE_FLAG_OFFSET + 1u;
}

int fa18_set_context_workspace_bit4(uint8_t *workspace_a, size_t workspace_a_size,
                                    uint8_t *workspace_b, size_t workspace_b_size) {
    if (!workspace_a || !workspace_b ||
        !has_lanes(workspace_a_size, FA18_CONTEXT_WORKSPACE_A_RECORD_STRIDE,
                   FA18_CONTEXT_WORKSPACE_A_RECORD_COUNT) ||
        !has_lanes(workspace_b_size, FA18_CONTEXT_WORKSPACE_B_RECORD_STRIDE,
                   FA18_CONTEXT_WORKSPACE_B_RECORD_COUNT))
        return -1;
    for (size_t index = 0; index != FA18_CONTEXT_WORKSPACE_A_RECORD_COUNT; ++index)
        workspace_a[index * FA18_CONTEXT_WORKSPACE_A_RECORD_STRIDE +
                    FA18_CONTEXT_WORKSPACE_FLAG_OFFSET] |= FA18_CONTEXT_WORKSPACE_READY_FLAG;
    for (size_t index = 0; index != FA18_CONTEXT_WORKSPACE_B_RECORD_COUNT; ++index)
        workspace_b[index * FA18_CONTEXT_WORKSPACE_B_RECORD_STRIDE +
                    FA18_CONTEXT_WORKSPACE_FLAG_OFFSET] |= FA18_CONTEXT_WORKSPACE_READY_FLAG;
    return 0;
}
