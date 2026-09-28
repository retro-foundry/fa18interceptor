#include "context_workspace_flags.h"

#include <assert.h>
#include <string.h>

int main(void) {
    static uint8_t workspace_a[FA18_CONTEXT_WORKSPACE_A_RECORD_COUNT *
                               FA18_CONTEXT_WORKSPACE_A_RECORD_STRIDE];
    static uint8_t workspace_b[FA18_CONTEXT_WORKSPACE_B_RECORD_COUNT *
                               FA18_CONTEXT_WORKSPACE_B_RECORD_STRIDE];

    memset(workspace_a, 0, sizeof workspace_a);
    memset(workspace_b, 0, sizeof workspace_b);
    workspace_a[1] = 0x81;
    workspace_b[1] = 0x02;
    assert(fa18_set_context_workspace_bit4(workspace_a, sizeof workspace_a,
                                           workspace_b, sizeof workspace_b) == 0);
    for (size_t index = 0; index != FA18_CONTEXT_WORKSPACE_A_RECORD_COUNT; ++index)
        assert(workspace_a[index * FA18_CONTEXT_WORKSPACE_A_RECORD_STRIDE + 1u] & 0x10u);
    for (size_t index = 0; index != FA18_CONTEXT_WORKSPACE_B_RECORD_COUNT; ++index)
        assert(workspace_b[index * FA18_CONTEXT_WORKSPACE_B_RECORD_STRIDE + 1u] & 0x10u);
    assert(workspace_a[1] == 0x91 && workspace_b[1] == 0x12);
    assert(!workspace_a[2] && !workspace_b[2]);
    assert(fa18_set_context_workspace_bit4(
               workspace_a,
               (FA18_CONTEXT_WORKSPACE_A_RECORD_COUNT - 1u) *
                   FA18_CONTEXT_WORKSPACE_A_RECORD_STRIDE + FA18_CONTEXT_WORKSPACE_FLAG_OFFSET,
                                           workspace_b, sizeof workspace_b) == -1);
    assert(fa18_set_context_workspace_bit4(
               workspace_a, sizeof workspace_a, workspace_b,
               (FA18_CONTEXT_WORKSPACE_B_RECORD_COUNT - 1u) *
                   FA18_CONTEXT_WORKSPACE_B_RECORD_STRIDE + FA18_CONTEXT_WORKSPACE_FLAG_OFFSET) == -1);
    assert(fa18_set_context_workspace_bit4(0, sizeof workspace_a,
                                           workspace_b, sizeof workspace_b) == -1);
    return 0;
}
