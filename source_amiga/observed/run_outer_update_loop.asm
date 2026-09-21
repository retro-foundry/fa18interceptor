; Byte-exact observed outer update-loop slice $C15D80-$C15DB3.
; Calls the complete static C0EFD4 parent update at $C15DA2.

                org     $C15D80

OUTER_HELPER_1                  equ $C53B48
OUTER_HELPER_2                  equ $C0E78A
OUTER_HELPER_3                  equ $C2F558
OUTER_HELPER_4                  equ $C53FB0
PARENT_UPDATE                   equ $C0EFD4
OUTER_HELPER_5                  equ $C53FC0
OUTER_HELPER_6                  equ $C1612C
OUTER_DELAY_ARGUMENT            equ $186A0

run_outer_update_loop_slice:
                jsr     OUTER_HELPER_1.l
                addq.l  #8,a7
.initial_delay:
                move.l  #OUTER_DELAY_ARGUMENT,-(a7)
                jsr     OUTER_HELPER_2.l
                addq.l  #4,a7
.outer_update_loop:
                jsr     OUTER_HELPER_3.l
                jsr     OUTER_HELPER_4.l
                jsr     PARENT_UPDATE.l
                jsr     OUTER_HELPER_5.l
                bsr.w   OUTER_HELPER_6
                bra.s   .outer_update_loop
