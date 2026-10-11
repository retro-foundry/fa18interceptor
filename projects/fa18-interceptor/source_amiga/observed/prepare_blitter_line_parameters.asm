; Byte-exact reconstruction of $C2FA7E-$C2FB4D (Hunk 36 +$5EE).
; Prepares Amiga blitter line-mode registers for the following hardware slice.

                org     $C2FA7E

LINE_LIMIT_WORD               equ $C45984
LINE_CONTROL_INPUT             equ $C45954
LINE_CONTROL_WORK              equ $C45956
LINE_SETUP_FALLBACK            equ $C2FA70
BLITTER_LINE_CONTROL_BASE      equ $0B00
BLITTER_LINE_OCTANT_X_POS      equ $10
BLITTER_LINE_OCTANT_X_NEG      equ $14
BLITTER_LINE_OCTANT_Y_NEG      equ 8
BLITTER_LINE_SIZE_BASE         equ $42

prepare_blitter_line_parameters:
                movea.w LINE_LIMIT_WORD.l,a2
                move.w  LINE_CONTROL_INPUT.l,LINE_CONTROL_WORK.l
                cmp.w   d1,d3
                beq.b   LINE_SETUP_FALLBACK
                bls.b   .reverse_y
.forward_y:
                move.w  d3,d5
                sub.w   d1,d5
                subq.w  #1,d5
                addq.w  #1,d1
                cmp.w   a2,d1
                bgt.b   .return
                movea.w d1,a1
                asl.w   #3,d1
                move.w  d1,d7
                add.w   d7,d7
                add.w   d7,d7
                add.w   d1,d7
                move.w  d2,d4
                sub.w   d0,d4
                move.w  d0,d6
                lsr.w   #3,d0
                add.w   d0,d7
                bra.b   .prepare_error
.return:
                rts
.reverse_y:
                move.w  d1,d5
                sub.w   d3,d5
                subq.w  #1,d5
                move.w  d0,d4
                sub.w   d2,d4
                addq.w  #1,d3
                cmp.w   a2,d3
                bgt.b   .return
                movea.w d3,a1
                asl.w   #3,d3
                move.w  d3,d7
                add.w   d7,d7
                add.w   d7,d7
                add.w   d3,d7
                move.w  d2,d6
                lsr.w   #3,d2
                add.w   d2,d7
.prepare_error:
                ext.l   d7
                moveq   #1,d1
                andi.w  #$F,d6
                ror.w   #4,d6
                addi.w  #BLITTER_LINE_CONTROL_BASE,d6
                tst.w   d4
                bmi.b   .negative_x_delta
                cmp.w   d5,d4
                bcs.b   .x_major_positive
                addi.w  #BLITTER_LINE_OCTANT_X_POS,d1
                bra.b   .choose_major_delta
.negative_x_delta:
                neg.w   d4
                cmp.w   d5,d4
                bcs.b   .x_major_negative
                addi.w  #BLITTER_LINE_OCTANT_X_NEG,d1
.choose_major_delta:
                suba.w  a1,a2
                cmp.w   a2,d5
                ble.b   .use_x_delta
                move.w  a2,d2
                move.w  d4,d3
                muls.w  d2,d3
                add.l   d3,d3
                divs.w  d5,d3
                asr.w   #1,d3
                bcc.b   .rounded_major_delta
                addq.w  #1,d3
.rounded_major_delta:
                movea.w d3,a2
                bra.b   .prepare_bresenham
.use_x_delta:
                movea.w d4,a2
                bra.b   .prepare_bresenham
.x_major_negative:
                addq.w  #BLITTER_LINE_OCTANT_Y_NEG,d1
.x_major_positive:
                exg.l   d4,d5
                suba.w  a1,a2
                cmp.w   a2,d4
                bgt.b   .prepare_bresenham
                movea.w d4,a2
.prepare_bresenham:
                add.w   d5,d5
                add.w   d5,d5
                add.w   d4,d4
                move.w  d5,d2
                sub.w   d4,d2
                bge.b   .error_nonnegative
                bset    #6,d1
.error_nonnegative:
                move.w  d5,d3
                add.w   d4,d4
                sub.w   d4,d5
                move.w  a2,d4
                asl.w   #6,d4
                addi.w  #BLITTER_LINE_SIZE_BASE,d4
                moveq   #-1,d0
                movea.w d5,a3
