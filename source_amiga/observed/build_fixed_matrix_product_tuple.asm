; Byte-exact $C0DAEE-$C0DB41 fixed-input matrix product and child invocation.
                org $C0DAEE
MATRIX_WORDS equ $C45BD8
CHILD_STATE_WORD equ $C45954
build_fixed_matrix_product_tuple:
 move.w #$e000,d2
 move.w #$3800,d3
 move.w #$e000,d4
 lea MATRIX_WORDS.l,a0
 move.w d2,d5
 move.w d3,d6
 move.w d4,d0
 muls.w (a0)+,d5
 muls.w (a0)+,d6
 muls.w (a0)+,d0
 add.l d6,d0
 add.l d5,d0
 asr.l #8,d0
 move.w d2,d5
 move.w d3,d6
 move.w d4,d1
 muls.w (a0)+,d5
 muls.w (a0)+,d6
 muls.w (a0)+,d1
 add.l d6,d1
 add.l d5,d1
 asr.l #8,d1
 muls.w (a0)+,d2
 muls.w (a0)+,d3
 muls.w (a0)+,d4
 add.l d3,d2
 add.l d4,d2
 asr.l #8,d2
 moveq #8,d6
 move.w #9,CHILD_STATE_WORD.l
 jsr $C2EC9C.l
 rts
