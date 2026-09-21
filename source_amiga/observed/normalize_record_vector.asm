; Byte-exact record-vector normalization stage $C25754-$C257D3.
; The bounded run003 frame-6000 trace covers this complete entry and return.

                org     $C25754

CALL_C1D974                     equ $C1D974
NORMALIZED_VECTOR_OUTPUT         equ $C45A4C
ZERO_MAGNITUDE_CONTINUATION      equ $C257D4

normalize_record_vector:
                link.w  a6,#-4
                movem.l 8(a6),d0/d5-d7
                move.w  8(a6),-2(a6)

normalize_vector_magnitude:
                tst.w   d0
                beq.b   ZERO_MAGNITUDE_CONTINUATION
                bgt.b   use_positive_magnitude
                neg.w   d0

use_positive_magnitude:
                move.w  d5,d2
                bge.b   use_positive_component_one
                neg.w   d2

use_positive_component_one:
                move.w  d6,d3
                bge.b   use_positive_component_two
                neg.w   d3

use_positive_component_two:
                move.w  d7,d4
                bge.b   calculate_normalization_scale
                neg.w   d4

calculate_normalization_scale:
                jsr     CALL_C1D974.l
                beq.b   ZERO_MAGNITUDE_CONTINUATION
                moveq   #8,d2

scale_magnitude_up:
                ext.l   d0

compare_magnitude_scale:
                cmp.l   d1,d0
                bgt.b   reduce_magnitude_scale
                asl.l   #2,d0
                addq.w  #2,d2
                bra.b   compare_magnitude_scale

check_magnitude_scale:
                cmp.l   d1,d0
                ble.b   compute_normalized_components

reduce_magnitude_scale:
                asr.l   #2,d0
                subq.w  #2,d2
                cmpi.w  #1,d2
                bgt.b   check_magnitude_scale

compute_normalized_components:
                asl.l   #2,d0
                addq.w  #2,d2
                asl.l   #8,d0
                divu.w  d1,d0
                muls.w  d0,d5
                muls.w  d0,d6
                muls.w  d0,d7
                asr.l   d2,d5
                asr.l   d2,d6
                asr.l   d2,d7
                tst.w   -2(a6)
                bge.b   store_normalized_components
                neg.w   d5
                neg.w   d6
                neg.w   d7

store_normalized_components:
                movem.w d5-d7,NORMALIZED_VECTOR_OUTPUT.l
                ext.l   d5
                ext.l   d6
                ext.l   d7

finish_record_vector_normalization:
                unlk    a6
                rts

; The zero-input continuation is reached both from the signed-magnitude test
; and from the scale-bound helper's zero result.
zero_magnitude_continuation:
                clr.w   d5
                clr.w   d6
                clr.w   d7
                bra.b   store_normalized_components
