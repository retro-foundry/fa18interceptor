; Byte-exact qualification-line formatter variant $C31FBC-$C32019.
; Layout and displayed-value ownership remain unresolved.

                org     $C31FBC

QUALIFICATION_MODE_FLAG          equ     $C45785
QUALIFICATION_LINE_BUFFER        equ     $C457FF
FORMAT_PACKED_VALUE              equ     $C25A08
SUBMIT_QUALIFICATION_LINE        equ     $C3271A
SUBMIT_MODE_ZERO_LINE            equ     $C32740

format_qualification_line_variant:
                ; LEA $C31958(PC),A1; preserved PC-relative encoding.
                dc.w    $43FA,$F99A
                lea     $1CD2.w,a4
                lea     $12.w,a5
                bra.b   .submit_layout
.mode_zero_layout:
                ; LEA $C31948(PC),A1; preserved PC-relative encoding.
                dc.w    $43FA,$F97C
                lea     $1A0E.w,a4
                lea     $1E.w,a5
.submit_layout:
                move.w  #$FCA,d6
                tst.b   QUALIFICATION_MODE_FLAG.l
                bne.b   .format_nonzero_mode
                move.w  #4,d5
                bra.w   SUBMIT_MODE_ZERO_LINE
.format_nonzero_mode:
                move.w  #0,d5
                addq.w  #3,d0
                move.b  #'K',4(a2)
                move.b  #'T',5(a2)
                move.b  #'S',6(a2)
                movem.l d0/a0-a2/a4-a5,-(a7)
                bsr.w   SUBMIT_QUALIFICATION_LINE
                movem.l (a7)+,d0/a0-a2/a4-a5
                move.w  #$F3A,d6
                move.w  #$C,d5
                bra.w   SUBMIT_QUALIFICATION_LINE
