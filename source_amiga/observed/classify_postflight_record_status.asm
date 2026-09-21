; Byte-exact static-only postflight record-status classification $C31518-$C315BF.

                org     $C31518

POSTFLIGHT_STATUS_BYTE           equ $C4586D
POSTFLIGHT_RECORD_INDEX           equ $C458DC
POSTFLIGHT_ATTRIBUTE_CLEAR       equ $C315AA
POSTFLIGHT_ATTRIBUTE_JOIN        equ $C315B0

classify_postflight_record_status:
                btst    #1,$20(a1)
                bne.w   POSTFLIGHT_ATTRIBUTE_JOIN
                ; MOVE.W 0(A1),D0 in the original zero-displacement form.
                dc.w    $3029,$0000
                andi.w  #$600,d0
                bne.w   POSTFLIGHT_ATTRIBUTE_JOIN
                btst    #7,$3(a1)
                bne.w   POSTFLIGHT_ATTRIBUTE_JOIN
                cmpi.b  #$30,d1
                beq.w   POSTFLIGHT_ATTRIBUTE_JOIN
                ; CMPI.B #0,D1. VASM otherwise substitutes TST.B D1.
                dc.w    $0C01,$0000
                bne.s   postflight_status_nonzero_attribute
                btst    #3,$1(a1)
                beq.s   postflight_status_index_match
                bra.s   POSTFLIGHT_ATTRIBUTE_JOIN
postflight_status_nonzero_attribute:
                btst    #3,$1(a1)
                beq.s   postflight_status_check_byte_62
                bset    #1,POSTFLIGHT_STATUS_BYTE.l
                bra.s   POSTFLIGHT_ATTRIBUTE_JOIN
postflight_status_check_byte_62:
                cmpi.b  #$15,$62(a1)
                beq.s   postflight_status_set_bit_three
                bset    #2,POSTFLIGHT_STATUS_BYTE.l
                bra.s   POSTFLIGHT_ATTRIBUTE_JOIN
postflight_status_set_bit_three:
                bset    #3,POSTFLIGHT_STATUS_BYTE.l
                bra.s   POSTFLIGHT_ATTRIBUTE_JOIN
postflight_status_index_match:
                move.b  $38(a1),d1
                andi.w  #$7F,d1
                cmp.w   POSTFLIGHT_RECORD_INDEX.l,d1
                bne.s   POSTFLIGHT_ATTRIBUTE_JOIN
                ; CMPI.B #0,$62(A1), preserving the original displacement form.
                dc.w    $0C29,$0000,$0062
                bne.s   postflight_status_set_bit_five
                bset    #4,POSTFLIGHT_STATUS_BYTE.l
                bra.s   POSTFLIGHT_ATTRIBUTE_JOIN
postflight_status_set_bit_five:
                bset    #5,POSTFLIGHT_STATUS_BYTE.l
                bra.s   POSTFLIGHT_ATTRIBUTE_JOIN

postflight_status_clear_attribute:
                ; BCLR #6,$20(A1), retained in the original indexed form.
                dc.w    $08A9,$0006,$0020
postflight_status_join:
                btst    #4,d3
                beq.s   postflight_status_values_ready
                ; BSET #6,$0(A1), retaining the original displacement form.
                dc.w    $08E9,$0006,$0000
postflight_status_values_ready:
                move.l  d5,d0
                move.l  a5,d1
