#include "display_record_iterator.h"

#include "rounded_signed_divide.h"

typedef enum { STANDARD, NEGATED, TRANSPOSED, NEGATED_TRANSPOSED } Adjustment;

typedef struct { int16_t d0, d1, d2; } Candidate;

static int16_t word_add(int16_t a, int16_t b) {
    return (int16_t)((uint16_t)a + (uint16_t)b);
}
static int16_t word_sub(int16_t a, int16_t b) {
    return (int16_t)((uint16_t)a - (uint16_t)b);
}
static int16_t word_neg(int16_t value) {
    return (int16_t)(UINT16_C(0) - (uint16_t)value);
}

static int candidate_in_bounds(Candidate value) {
    return value.d2 >= 0 && value.d0 <= value.d2 &&
           word_neg(value.d0) <= value.d2 && value.d1 <= value.d2 &&
           word_neg(value.d1) <= value.d2;
}

/* Shared arithmetic of `$C2EA5A/$C2EAD0/$C2EB4C/$C2EBC2`. */
static int adjust(Adjustment kind, const int16_t neighbour[3], int16_t d3,
                  int16_t d4, int16_t d5, Candidate *result) {
    int16_t d0 = neighbour[0], d1 = neighbour[1], d2 = neighbour[2];
    int16_t d6, quotient;
    int transposed = kind == TRANSPOSED || kind == NEGATED_TRANSPOSED;

    d3 = word_sub(d3, 1);
    d4 = word_sub(d4, 1);
    if (kind == NEGATED) { d0 = word_neg(d0); d3 = word_neg(d3); }
    if (kind == NEGATED_TRANSPOSED) { d1 = word_neg(d1); d4 = word_neg(d4); }
    d6 = word_sub(d2, d5);
    if (!transposed) {
        d0 = word_neg(word_sub(d2, d0));
        d3 = word_add(word_sub(d3, d5), d0);
        if (!d3) return 1;
        d4 = word_neg(word_sub(d4, d1));
        if (fa18_round_signed_divide((int32_t)d4 * d0, d3, &quotient)) return -1;
        d1 = word_sub(d1, quotient);
        if (fa18_round_signed_divide((int32_t)d6 * d0, d3, &quotient)) return -1;
        d2 = word_sub(d2, quotient);
        d0 = d2;
        if (kind == NEGATED) d0 = word_neg(d0);
    } else {
        d1 = word_neg(word_sub(d2, d1));
        d4 = word_add(word_sub(d4, d5), d1);
        if (!d4) return 1;
        d3 = word_neg(word_sub(d3, d0));
        if (fa18_round_signed_divide((int32_t)d3 * d1, d4, &quotient)) return -1;
        d0 = word_sub(d0, quotient);
        if (fa18_round_signed_divide((int32_t)d6 * d1, d4, &quotient)) return -1;
        d2 = word_sub(d2, quotient);
        d1 = d2;
        if (kind == NEGATED_TRANSPOSED) d1 = word_neg(d1);
    }
    *result = (Candidate){d0, d1, d2};
    return candidate_in_bounds(*result) ? 0 : 1;
}

static int publish(Candidate candidate, int16_t workspace[2]) {
    FA18DisplayRecordPair pair;
    if (fa18_project_adjusted_display_pair(candidate.d0, candidate.d1,
                                           candidate.d2, &pair)) return -1;
    workspace[0] = pair.x;
    workspace[1] = pair.y;
    return 0;
}

static void clear_record(int16_t records[8][8], int16_t workspace[2],
                         uint16_t index) {
    workspace[0] = 0;
    workspace[1] = 0;
    records[index][7] = 0;
}

int fa18_iterate_display_records(int16_t records[8][8], int16_t workspace[8][2],
                                 int16_t scratch[8], int16_t adjustment_gate_5aca) {
    int defer = 0;
    if (!records || !workspace || !scratch) return -1;
    for (uint16_t index = 0; index < 8; ++index) {
        uint16_t selected = (index & 1u) ? ((index + 1u) & 7u) : index;
        uint16_t neighbour = (index & 1u) ? index - 1u : (index + 2u) & 7u;
        const int16_t *current = records[selected];
        const int16_t *other = records[neighbour];
        Candidate candidate;
        int16_t d3, d4, d5, d2, d6;
        int status;

        if (defer) { defer = 0; continue; }
        d3 = word_add(current[0], 1); d4 = word_add(current[1], 1); d5 = current[2];
        if (d3 < d5) goto case2;
        d2 = other[0]; d6 = other[2];
        if (d6 <= d2) goto failed;
        status = adjust(STANDARD, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_1;
        if (adjustment_gate_5aca >= 0) goto case3;
        d2 = word_neg(d2);
        if (d6 <= d2) goto case3;
        status = adjust(NEGATED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (status) goto case3;
        goto publish_3;

case2:
        d6 = word_neg(d3);
        if (d6 < d5) goto case5;
        d2 = other[0]; d6 = other[2]; d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(NEGATED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_3;
        if (adjustment_gate_5aca >= 0) goto case3;
        d2 = word_neg(d2);
        if (d6 <= d2) goto case3;
        status = adjust(STANDARD, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_1;

case3:
        if (d4 >= 0) goto case4;
        d6 = word_neg(d4);
        if (d6 < d5) goto failed;
        d2 = other[1]; d6 = other[2]; d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(NEGATED_TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_2;
        if (adjustment_gate_5aca >= 0 || d4 < d5) goto failed;
        d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status) { if (status < 0) return -1; goto failed; }
        goto publish_0;

case4:
        if (d4 < d5) goto failed;
        d2 = other[1]; d6 = other[2];
        if (d6 <= d2) goto failed;
        status = adjust(TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_0;
        if (adjustment_gate_5aca >= 0) goto failed;
        d6 = word_neg(d4);
        if (d6 < d5) goto failed;
        d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(NEGATED_TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status) { if (status < 0) return -1; goto failed; }
        goto publish_2;

case5:
        if (d4 < d5) goto case6;
        d2 = other[1]; d6 = other[2];
        if (d6 <= d2) goto failed;
        status = adjust(TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_0;
        if (adjustment_gate_5aca >= 0) goto case7;
        d2 = word_neg(d2);
        if (d6 <= d2) goto case7;
        status = adjust(NEGATED_TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status) { if (status < 0) return -1; goto case7; }
        goto publish_2;

case6:
        d6 = word_neg(d4);
        if (d6 < d5) goto deferred_or_failed;
        d2 = other[1]; d6 = other[2]; d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(NEGATED_TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_2;
        if (adjustment_gate_5aca >= 0) goto case7;
        d2 = word_neg(d2);
        if (d6 <= d2) goto case7;
        status = adjust(TRANSPOSED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_0;

case7:
        if (d3 >= 0) goto case8;
        d6 = word_neg(d3);
        if (d6 < d5) goto failed;
        d2 = other[0]; d6 = other[2]; d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(NEGATED, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_3;
        if (adjustment_gate_5aca >= 0 || d3 < d5) goto failed;
        d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(STANDARD, other, d3, d4, d5, &candidate);
        if (status) { if (status < 0) return -1; goto failed; }
        goto publish_1;

case8:
        if (d3 < d5) goto failed;
        d2 = other[0]; d6 = other[2];
        if (d6 <= d2) goto failed;
        status = adjust(STANDARD, other, d3, d4, d5, &candidate);
        if (status < 0) return -1;
        if (!status) goto publish_1;
        if (adjustment_gate_5aca >= 0) goto failed;
        d6 = word_neg(d3);
        if (d6 < d5) goto failed;
        d2 = word_neg(d2);
        if (d6 <= d2) goto failed;
        status = adjust(NEGATED, other, d3, d4, d5, &candidate);
        if (status) { if (status < 0) return -1; goto failed; }
        goto publish_3;

publish_0: scratch[0] = word_add(scratch[0], 1); scratch[4] = (int16_t)index; goto project;
publish_1: scratch[1] = word_add(scratch[1], 1); scratch[5] = (int16_t)index; goto project;
publish_2: scratch[2] = word_add(scratch[2], 1); scratch[6] = (int16_t)index; goto project;
publish_3: scratch[3] = word_add(scratch[3], 1); scratch[7] = (int16_t)index;
project:
        if (publish(candidate, workspace[index])) return -1;
        continue;
deferred_or_failed:
        if (d5 >= 0) {
            if (index & 1u) defer = 1;
            continue;
        }
failed:
        clear_record(records, workspace[index], index);
    }
    return 0;
}
