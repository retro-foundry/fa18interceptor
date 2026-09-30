#ifndef FA18_GAME_POSTFLIGHT_VARIANTS_H
#define FA18_GAME_POSTFLIGHT_VARIANTS_H

#include "memory.h"

typedef struct PostflightVariantWork {
    gaddr table, vector_stream;
    gaddr record;
    int32_t vector[3];
    int32_t source_x, source_z;
    int32_t screen_x, screen_y;
    int16_t submit_x, submit_y;
    uint16_t record_word;
    uint16_t records_seen;
    int has_vector;
} PostflightVariantWork;

/* Drawing heads of the two postflight dispatcher targets. Both fall through
 * to the common $C31392 tail, which is handled separately. */
void draw_postflight_tuple_pairs(void); /* $C3129A-$C3130E */
void draw_postflight_fixed_quad(void); /* $C31312-$C31392 */

/* $C31392-$C3141D: shared gate, point table, and three-long vector fetch.
 * Returns zero only when the source returns at $C31224. */
int begin_postflight_variant_tail(PostflightVariantWork *work);

/* $C3141E-$C3149B: normalize x/z and select a control record. Returns one
 * only on the branch to $C3149C; zero takes the $C31714 skip path. */
int select_postflight_variant_record(PostflightVariantWork *work);

/* $C3149C-$C315BF: source-order attribute guards and status-bit writes for
 * a record accepted by select_postflight_variant_record. */
void classify_postflight_variant_record(PostflightVariantWork *work);

/* $C315C0-$C3170D: second normalization, colour choice, and optional point
 * submission. Returns zero only for the $C31714 record-skip edge. */
int submit_postflight_variant_record(PostflightVariantWork *work);

/* $C3170E-$C31721: publish bit 5 of the current record word and load the
 * next three-long vector for the shared loop. */
void advance_postflight_variant_record(PostflightVariantWork *work, int marked);

/* Source loop from $C31410 back through $C31721, stopping at the status
 * resolution entry $C31722 when the three-long terminator is reached. */
void process_postflight_variant_records(PostflightVariantWork *work);

#endif
