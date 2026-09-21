; Byte-exact static-only postflight renderer submission tail $C316C0-$C31721.

                org     $C316C0

POSTFLIGHT_TABLE_SELECT_WORD     equ $C4566C
POSTFLIGHT_TABLE_LIMIT_A         equ $C4E744
POSTFLIGHT_TABLE_LIMIT_B         equ $C4E76C
POSTFLIGHT_VERTICAL_OFFSET       equ $C458D8
POSTFLIGHT_SHARED_RENDERER       equ $C2F5F4
POSTFLIGHT_ADJACENT_RENDERER     equ $C2F60A
POSTFLIGHT_LOOP_REJECT           equ $C3170E
POSTFLIGHT_LOOP_NEXT             equ $C31410

submit_postflight_renderer_record:
                tst.w   POSTFLIGHT_TABLE_SELECT_WORD.l
                bne.s   postflight_submit_limit_b
                cmpa.l  #POSTFLIGHT_TABLE_LIMIT_A,a2
                bra.s   postflight_submit_limit_check
postflight_submit_limit_b:
                cmpa.l  #POSTFLIGHT_TABLE_LIMIT_B,a2
postflight_submit_limit_check:
                bge.s   POSTFLIGHT_LOOP_REJECT
                move.w  d3,-(a7)
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                btst    #0,d7
                bne.s   postflight_submit_flagged
                move.w  d0,(a2)+
                move.w  d1,(a2)+
                movem.l d6/a0-a2,-(a7)
                jsr     POSTFLIGHT_SHARED_RENDERER.l
                bra.s   postflight_submit_restore
postflight_submit_flagged:
                move.w  d0,(a2)
                ori.w   #$8000,(a2)+
                move.w  d1,(a2)+
                movem.l d6/a0-a2,-(a7)
                jsr     POSTFLIGHT_ADJACENT_RENDERER.l
postflight_submit_restore:
                movem.l (a7)+,d6/a0-a2
                move.w  (a7)+,d3
postflight_submit_reject:
                bset    #5,d3
                bra.s   postflight_submit_loop_update
postflight_submit_clear_reject:
                bclr    #5,d3
postflight_submit_loop_update:
                move.w  d3,(a0)+
                addq.w  #2,a0
                addq.w  #1,d6
                bra.w   POSTFLIGHT_LOOP_NEXT
