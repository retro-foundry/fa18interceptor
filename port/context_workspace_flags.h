#ifndef FA18_CONTEXT_WORKSPACE_FLAGS_H
#define FA18_CONTEXT_WORKSPACE_FLAGS_H

#include <stddef.h>
#include <stdint.h>

enum {
    FA18_CONTEXT_WORKSPACE_A_RECORD_COUNT = 16,
    FA18_CONTEXT_WORKSPACE_A_RECORD_STRIDE = 0x200,
    FA18_CONTEXT_WORKSPACE_B_RECORD_COUNT = 16,
    FA18_CONTEXT_WORKSPACE_B_RECORD_STRIDE = 0x20,
    FA18_CONTEXT_WORKSPACE_FLAG_OFFSET = 1,
    FA18_CONTEXT_WORKSPACE_READY_FLAG = 0x10
};

/* `$C1CA82-$C1CB13`: OR bit four into byte one of each of the sixteen
 * `$C46184` records and each of the sixteen `$C48184` workspace lanes.
 * Both buffers are caller-owned mutable source workspaces. */
int fa18_set_context_workspace_bit4(uint8_t *workspace_a, size_t workspace_a_size,
                                    uint8_t *workspace_b, size_t workspace_b_size);

#endif
