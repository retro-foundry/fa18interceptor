; Byte-exact observed tail $C082B8-$C08323.  It initializes the listed byte
; latches to three and clears two additional fields when the gate is clear.

                org     $C082B8

CONTEXT_LATCH_37                equ     $C45837
CONTEXT_LATCH_36                equ     $C45836
CONTEXT_LATCH_39                equ     $C45839
CONTEXT_LATCH_3A                equ     $C4583A
CONTEXT_LATCH_3E                equ     $C4583E
CONTEXT_LATCH_3F                equ     $C4583F
CONTEXT_LATCH_40                equ     $C45840
CONTEXT_LATCH_41                equ     $C45841
CONTEXT_LATCH_3B                equ     $C4583B
CONTEXT_LATCH_3D                equ     $C4583D
CONTEXT_LATCH_43                equ     $C45843
CONTEXT_LATCH_44                equ     $C45844
CONTEXT_LATCH_45                equ     $C45845
CONTEXT_LATCH_3C                equ     $C4583C
CONTEXT_CLEAR_GATE              equ     $C457C0
CONTEXT_CLEAR_WORD              equ     $C458D8
CONTEXT_CLEAR_LONG              equ     $C45918

initialize_c082b8_context_latches:
                moveq   #3,d4
                move.b  d4,CONTEXT_LATCH_37.l
                move.b  d4,CONTEXT_LATCH_36.l
                move.b  d4,CONTEXT_LATCH_39.l
                move.b  d4,CONTEXT_LATCH_3A.l
                move.b  d4,CONTEXT_LATCH_3E.l
                move.b  d4,CONTEXT_LATCH_3F.l
                move.b  d4,CONTEXT_LATCH_40.l
                move.b  d4,CONTEXT_LATCH_41.l
                move.b  d4,CONTEXT_LATCH_3B.l
                move.b  d4,CONTEXT_LATCH_3D.l
                move.b  d4,CONTEXT_LATCH_43.l
                move.b  d4,CONTEXT_LATCH_44.l
                move.b  d4,CONTEXT_LATCH_45.l
                move.b  d4,CONTEXT_LATCH_3C.l
                tst.b   CONTEXT_CLEAR_GATE.l
                bne.b   $C08322
                clr.w   CONTEXT_CLEAR_WORD.l
                clr.l   CONTEXT_CLEAR_LONG.l
                rts
