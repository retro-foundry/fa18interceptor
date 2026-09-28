#include "display_record_selector.h"

static void append_relative(const int16_t workspace[8][2], uint16_t first,
                            uint16_t second, int16_t words[16], uint16_t *at) {
    words[(*at)++] = (int16_t)(319 - workspace[first][0]);
    words[(*at)++] = (int16_t)(179 - workspace[first][1]);
    words[(*at)++] = (int16_t)(319 - workspace[second][0]);
    words[(*at)++] = (int16_t)(179 - workspace[second][1]);
}
static void append_zero(int16_t words[16], uint16_t *at) {
    words[(*at)++] = 0; words[(*at)++] = 0;
}
static void append_right_zero(int16_t words[16], uint16_t *at) {
    words[(*at)++] = 319; words[(*at)++] = 0;
}
static void append_right_bottom(int16_t words[16], uint16_t *at) {
    words[(*at)++] = 319; words[(*at)++] = 179;
}
static void append_zero_bottom(int16_t words[16], uint16_t *at) {
    words[(*at)++] = 0; words[(*at)++] = 179;
}

int fa18_select_display_record_pairs(const int16_t workspace[8][2],
                                     const int16_t scratch[8],
                                     const FA18DisplayRecordSelectorInput *input,
                                     FA18DisplayRecordSelectorOutput *output) {
    uint16_t first, second, at = 1;
    int branch;

    if (!workspace || !scratch || !input || !output) return -1;
    if (scratch[1]) {
        if (scratch[3]) { first = (uint16_t)scratch[5]; second = (uint16_t)scratch[7]; branch = 1; }
        else if (scratch[0]) { first = (uint16_t)scratch[5]; second = (uint16_t)scratch[4]; branch = 2; }
        else if (scratch[2]) { first = (uint16_t)scratch[5]; second = (uint16_t)scratch[6]; branch = 3; }
        else goto reject;
    } else if (scratch[0]) {
        if (scratch[3]) { first = (uint16_t)scratch[4]; second = (uint16_t)scratch[7]; branch = 4; }
        else if (scratch[2]) { first = (uint16_t)scratch[4]; second = (uint16_t)scratch[6]; branch = 5; }
        else goto reject;
    } else if (scratch[2]) {
        if (scratch[3]) { first = (uint16_t)scratch[6]; second = (uint16_t)scratch[7]; branch = 6; }
        else goto reject;
    } else {
        branch = 7;
        first = second = 0;
    }
    if ((branch != 7 && (first >= 8 || second >= 8))) return -1;

    switch (branch) {
    case 1:
        output->words[0] = 4; append_relative(workspace, first, second, output->words, &at);
        append_right_bottom(output->words, &at); append_zero_bottom(output->words, &at);
        if (input->mode_flag || !(input->sequence_flags & 2u)) {
            at = 5; append_right_zero(output->words, &at); append_zero(output->words, &at);
        }
        break;
    case 2:
        output->words[0] = 5; append_relative(workspace, first, second, output->words, &at);
        append_right_zero(output->words, &at); append_right_bottom(output->words, &at); append_zero_bottom(output->words, &at);
        if (!(input->sequence_flags & 2u)) { at = 1; output->words[at++] = 3; at += 4; append_zero(output->words, &at); }
        break;
    case 3:
        output->words[0] = 5; append_relative(workspace, first, second, output->words, &at);
        append_right_bottom(output->words, &at); append_right_zero(output->words, &at); append_zero(output->words, &at);
        if (input->sequence_flags & 2u) { at = 1; output->words[at++] = 3; at += 4; append_zero_bottom(output->words, &at); }
        break;
    case 4:
        output->words[0] = 5; append_relative(workspace, first, second, output->words, &at);
        append_right_bottom(output->words, &at); append_zero_bottom(output->words, &at); append_zero(output->words, &at);
        if (!(input->sequence_flags & 2u)) { at = 1; output->words[at++] = 3; at += 4; append_right_zero(output->words, &at); }
        break;
    case 5:
        output->words[0] = 4; append_relative(workspace, first, second, output->words, &at);
        append_zero_bottom(output->words, &at); append_zero(output->words, &at);
        if (input->threshold_source < 0x3840) { at = 5; append_right_bottom(output->words, &at); append_right_zero(output->words, &at); }
        break;
    case 6:
        output->words[0] = 5; append_relative(workspace, first, second, output->words, &at);
        append_right_zero(output->words, &at); append_zero(output->words, &at); append_zero_bottom(output->words, &at);
        if (input->sequence_flags & 2u) { at = 1; output->words[at++] = 3; at += 4; append_right_bottom(output->words, &at); }
        break;
    default:
        output->words[0] = 4; append_zero(output->words, &at); append_right_zero(output->words, &at);
        append_right_bottom(output->words, &at); append_zero_bottom(output->words, &at);
        if ((input->mode_flag ? input->mode_nonzero_threshold_source : input->mode_zero_threshold_source) <= 0x3840)
            goto reject;
        break;
    }
    output->selection_word_a = 1; output->selection_word_b = 1;
    output->selection_long = 0; output->selection_flag = 1;
    return 0;
reject:
    output->selection_flag = 0;
    return 1;
}
