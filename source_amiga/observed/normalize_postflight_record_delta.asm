; Byte-exact partially runtime-observed postflight normalization/rejection slice $C3141E-$C3149B.

                org     $C3141E

POSTFLIGHT_RECORD_BASE           equ $C46184
POSTFLIGHT_RECORD_OFFSET          equ $C458DE
POSTFLIGHT_RECORD_REJECT         equ $C31714

normalize_postflight_record_delta:
                exg.l   d1,d2
                move.l  d0,d5
                movea.l d1,a5
                move.w  #$D,d4
                asr.l   d4,d0
                bcc.s   postflight_normalize_d0_ready
                addq.l  #1,d0
postflight_normalize_d0_ready:
                asr.l   d4,d1
                bcc.s   postflight_normalize_d1_ready
                addq.l  #1,d1
postflight_normalize_d1_ready:
                move.w  (a0),d3
                btst    #4,d3
                beq.w   POSTFLIGHT_RECORD_REJECT
                move.w  d3,d4
                andi.w  #$FF00,d4
                add.w   d4,d4
                lea     POSTFLIGHT_RECORD_BASE.l,a1
                adda.w  d4,a1
                cmp.w   POSTFLIGHT_RECORD_OFFSET.l,d4
                beq.s   postflight_normalize_clear_record_bit
                btst    #6,$1(a1)
                beq.s   postflight_normalize_clear_record_bit
                btst    #7,$3(a1)
                bne.s   postflight_normalize_clear_record_bit
                neg.l   d0
                cmpi.l  #$1B,d0
                bgt.s   postflight_normalize_recheck_flag
                cmpi.l  #-$1B,d0
                blt.s   postflight_normalize_recheck_flag
                neg.l   d1
                cmpi.l  #$16,d1
                bge.s   postflight_normalize_recheck_flag
                cmpi.l  #-$10,d1
                bge.s   postflight_normalize_continue
postflight_normalize_recheck_flag:
                btst    #4,d3
                beq.w   POSTFLIGHT_RECORD_REJECT
postflight_normalize_clear_record_bit:
                ; BCLR #6,(A1) encoded with the original absolute extension.
                dc.w    $08A9,$0006,$0000
                bra.w   POSTFLIGHT_RECORD_REJECT
postflight_normalize_continue:
