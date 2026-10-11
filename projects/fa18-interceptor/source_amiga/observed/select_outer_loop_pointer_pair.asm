; Byte-exact outer-loop pointer-pair selector $C2F558-$C2F581.
; Complete no-input packet: C2F558 -> C15D9C in nine instructions.

                org     $C2F558

OUTER_SELECTED_INDEX             equ $C4566C
OUTER_POINTER_PAIR_BASE_0        equ $C4566E
OUTER_POINTER_PAIR_BASE_1        equ $C4568E
OUTER_SELECTED_POINTER_0         equ $C456B6
OUTER_SELECTED_POINTER_1         equ $C456BA
OUTER_PAIR_0_OFFSET              equ 0
OUTER_PAIR_1_OFFSET_0            equ $10
OUTER_PAIR_1_OFFSET_1            equ $14
ADDA_W_IMMEDIATE_A0_OPCODE       equ $D0FC
ADDA_W_IMMEDIATE_A1_OPCODE       equ $D2FC

select_outer_loop_pointer_pair:
                lea.l   OUTER_POINTER_PAIR_BASE_0.l,a0
                lea.l   OUTER_POINTER_PAIR_BASE_1.l,a1
                tst.w   OUTER_SELECTED_INDEX.l
                beq.s   .store_pair
                ; VASM rewrites these to LEA; retain original ADDA.W encodings.
                dc.w    ADDA_W_IMMEDIATE_A0_OPCODE,OUTER_PAIR_1_OFFSET_0
                dc.w    ADDA_W_IMMEDIATE_A1_OPCODE,OUTER_PAIR_1_OFFSET_1
.store_pair:
                move.l  a0,OUTER_SELECTED_POINTER_0.l
                move.l  a1,OUTER_SELECTED_POINTER_1.l
                rts
