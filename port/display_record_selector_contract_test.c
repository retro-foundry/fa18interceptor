#include "display_record_selector.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const int16_t workspace[8][2] = {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,90}, {319,90}, {0,0}, {0,0}
    };
    const int16_t scratch[8] = {0, 1, 0, 1, 0, 5, 0, 4};
    const int16_t expected[] = {4, 0, 89, 319, 89, 319, 0, 0, 0};
    const FA18DisplayRecordSelectorInput input = {0, 0, 0, 0, 0};
    FA18DisplayRecordSelectorOutput output;

    memset(&output, 0x55, sizeof output);
    /* `build/run075_prepared_c0d7e0/`: first selected branch, extended form. */
    assert(fa18_select_display_record_pairs(workspace, scratch, &input, &output) == 0);
    for (uint16_t index = 0; index < sizeof expected / sizeof expected[0]; ++index)
        assert(output.words[index] == expected[index]);
    assert(output.selection_word_a == 1 && output.selection_word_b == 1 &&
           output.selection_long == 0 && output.selection_flag == 1);
    assert(fa18_select_display_record_pairs(NULL, scratch, &input, &output) == -1);
    puts("display-record selector contract passed");
    return 0;
}
