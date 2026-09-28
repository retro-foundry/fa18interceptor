#include "c1f99a_record_transform.h"
#include "extended_record_dispatch.h"

#include <assert.h>
#include <string.h>

int main(void) {
    /* `$C1F9BC-$C1FA90`: descriptor +10/+D7 source and `$C48390 + D7`
     * destination, with identity 8.8 matrices retaining two source triples. */
    uint8_t descriptor[32] = {0};
    uint8_t workspace[24] = {0};
    const FA18TransformMatrix identity = {{{256, 0, 0}, {0, 256, 0}, {0, 0, 256}}};
    const uint8_t expected[] = {0, 1, 0, 2, 0, 3, 0xff, 0xfe, 0, 5, 0, 6};
    FA18C1F99ARecordTransformInput input = {
        descriptor, sizeof descriptor, 0, 2, 2, 0, 8,
        {0, 0, 0}, {0, 0, 0}, identity, identity, workspace, sizeof workspace
    };
    descriptor[7] = 1;
    descriptor[12] = 0; descriptor[13] = 1;
    descriptor[14] = 0; descriptor[15] = 2;
    descriptor[16] = 0; descriptor[17] = 3;
    descriptor[18] = 0xff; descriptor[19] = 0xfe;
    descriptor[20] = 0; descriptor[21] = 5;
    descriptor[22] = 0; descriptor[23] = 6;
    assert(fa18_transform_c1f99a_record(&input) == 0);
    assert(memcmp(workspace + 2, expected, sizeof expected) == 0);
    descriptor[7] = 0;
    assert(fa18_transform_c1f99a_record(&input) == -1);
    descriptor[7] = 1;
    memset(workspace, 0, sizeof workspace);
    {
        const uint8_t stream[] = {0, 2, 0, 2, 0x80, 0x34};
        FA18ExtendedRecordDispatchResult dispatch_result;
        FA18ExtendedRecordDispatchRoute route;
        int target(void *context, uint16_t selector, int16_t *status);
        FA18ExtendedRecordDispatchInput dispatch_input = {
            stream, sizeof stream, 0x2000, 0x2000, INT16_MAX, 0, 0,
            fa18_transform_c1f99a_record_callback, target, 0, &input, 0
        };
        assert(fa18_dispatch_extended_record(&dispatch_input, &dispatch_result, &route) == 0);
        assert(route == FA18_EXTENDED_RECORD_DISPATCH_CONTINUE &&
               dispatch_result.selector == 0x0034 &&
               memcmp(workspace + 2, expected, sizeof expected) == 0);
    }
    return 0;
}

int target(void *context, uint16_t selector, int16_t *status) {
    (void)context;
    assert(selector == 0x0034);
    *status = 0;
    return 0;
}
