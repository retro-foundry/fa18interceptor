; Byte-exact prepared-record triple dispatch $C200A8-$C200F5.
; Calls and record-word ownership remain structural.

                org     $C200A8

PREPARED_TRIPLE_COUNT           equ $C4BF92
SELECTED_RECORD_WORD            equ $C45954
COMPONENT_TEST_SELECTOR         equ $C1FB82
DISPLAY_STAGE_HELPER            equ $C2469E

dispatch_c200a8_prepared_record_triples:
                move.w  d7,PREPARED_TRIPLE_COUNT.l
                movea.w (a2)+,a3
                move.w  a3,d7
                blt.b   $C200DC
                addq.w  #1,-50(a6)
                jsr     COMPONENT_TEST_SELECTOR.l
                beq.b   $C200CC
                move.w  a3,d1
                andi.w  #$4000,d1
                beq.b   $C200F2
                move.w  (a2)+,d7
                bra.b   $C200DC
                addq.w  #1,-52(a6)
                move.w  a3,d1
                move.w  d1,d7
                andi.w  #$4000,d1
                beq.b   $C200DC
                addq.w  #2,a2
                move.w  d7,SELECTED_RECORD_WORD.l
                movem.l a5/a2/a1,-(sp)
                jsr     DISPLAY_STAGE_HELPER.l
                movem.l (sp)+,a1/a2/a5
                rts
                moveq   #-1,d0
                rts
