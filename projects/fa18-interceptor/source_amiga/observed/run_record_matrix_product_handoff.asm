; Byte-exact matrix-product handoff $C2D6FC-$C2D70B.

                org     $C2D6FC

CALL_SIGNED_MATRIX_PRODUCT      equ     $C2DEE0
CONTINUE_RECORD_FLAG_CLEAR      equ     $C2D76E

run_record_matrix_product_handoff:
                lea.l   $80(a1),a4
                bsr.w   CALL_SIGNED_MATRIX_PRODUCT
                dc.w    $08A9,$0004,$0003 ; bclr.b #4,3(a1); retain original EA
                beq.b   CONTINUE_RECORD_FLAG_CLEAR
