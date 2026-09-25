; Byte-exact packed-value transform setup $C32A44-$C32A95.
; This is static/dataflow reconstruction; display-field ownership is unresolved.

                org     $C32A44

POSTFLIGHT_PACKED_INPUT          equ     $C45B1E
PACKED_BCD_CONVERTER             equ     $C25A08
POSTFLIGHT_COORDINATE_TABLE      equ     $C3198C
POSTFLIGHT_STATUS_SCRATCH        equ     $C457FA
POSTFLIGHT_TRANSFORM_FORMAT      equ     $C32AD0

prepare_postflight_packed_transform_value:
                ext.l   d2
                move.l  d2,POSTFLIGHT_PACKED_INPUT.l
                jsr     PACKED_BCD_CONVERTER.l
                ; LEA $C3198C(PC),A1; retain original PC-relative encoding.
                dc.w    $43FA,$EF38
                lea.l   POSTFLIGHT_STATUS_SCRATCH.l,a2
                move.w  d3,d2
                lea.l   1(a2,d2.w),a0
                move.w  d0,d3
                andi.w  #$F,d3
                asr.w   #2,d3
                addq.w  #2,d3
                add.w   d3,d3
                add.w   d3,d3
                adda.w  d3,a1
                asr.w   #3,d0
                ext.l   d0
                asl.w   #3,d1
                move.w  d1,d3
                add.w   d3,d3
                add.w   d3,d3
                add.w   d3,d1
                ext.l   d1
                movea.l d1,a4
                bclr    #0,d0
                adda.l  d0,a4
                swap    d0
                move.w  d2,d0
                moveq   #0,d6
                moveq   #0,d7
                moveq   #1,d4
                bra.b   POSTFLIGHT_TRANSFORM_FORMAT

