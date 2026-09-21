; Byte-exact runtime-observed second postflight normalization $C315C0-$C31611.

                org     $C315C0

POSTFLIGHT_RECORD_BASE           equ $C46184
POSTFLIGHT_RECORD_OFFSET          equ $C458DE
POSTFLIGHT_RECORD_REJECT         equ $C31714

normalize_postflight_record_delta_second:
                move.l  a0,-(a7)
                lea     POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a0
                move.b  $63(a0),d4
                movea.l (a7)+,a0
                andi.b  #$F,d4
                ext.w   d4
                asr.l   d4,d0
                bcc.s   postflight_second_d0_ready
                addq.l  #1,d0
postflight_second_d0_ready:
                asr.l   d4,d1
                bcc.s   postflight_second_d1_ready
                addq.l  #1,d1
postflight_second_d1_ready:
                neg.l   d0
                cmpi.l  #$1B,d0
                bgt.w   POSTFLIGHT_RECORD_REJECT
                cmpi.l  #-$1B,d0
                blt.w   POSTFLIGHT_RECORD_REJECT
                neg.l   d1
                cmpi.l  #$16,d1
                bge.w   POSTFLIGHT_RECORD_REJECT
                cmpi.l  #-$10,d1
                blt.w   POSTFLIGHT_RECORD_REJECT
