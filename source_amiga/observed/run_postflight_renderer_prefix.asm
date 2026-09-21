; Byte-exact static-only postflight renderer prefix $C31392-$C3141D.

                org     $C31392

POSTFLIGHT_RETURN                equ $C31224
POSTFLIGHT_GATE_BYTE             equ $C45838
POSTFLIGHT_TABLE_BASE            equ $C4E71C
POSTFLIGHT_TABLE_SELECT_WORD     equ $C4566C
POSTFLIGHT_RENDERER_SELECTOR     equ $C45954
POSTFLIGHT_COUNTER_BYTE          equ $C45883
POSTFLIGHT_STATUS_BYTE           equ $C4586D
POSTFLIGHT_TRIPLE_BASE           equ $C4E2BC
POSTFLIGHT_DEEPER_ZERO_TARGET    equ $C31722
SUBMIT_ADJACENT_RENDERER_VALUES  equ $C2F60A
SUBMIT_SHARED_RENDERER_VALUES    equ $C2F5F4

run_postflight_renderer_prefix:
                tst.b   POSTFLIGHT_GATE_BYTE.l
                beq.w   POSTFLIGHT_RETURN
                lea     POSTFLIGHT_TABLE_BASE.l,a2
                tst.w   POSTFLIGHT_TABLE_SELECT_WORD.l
                beq.s   postflight_renderer_table_selected
                ; ADDA.W #$28,A2. VASM otherwise selects a different form.
                dc.w    $D4FC,$0028
postflight_renderer_table_selected:
                move.w  #0,POSTFLIGHT_RENDERER_SELECTOR.l
                moveq   #$A,d2
postflight_renderer_table_loop:
                move.w  (a2)+,d0
                bge.s   postflight_renderer_table_shared
                cmpi.w  #-$1,d0
                beq.s   postflight_renderer_table_finished
                bclr    #15,d0
                move.w  (a2)+,d1
                move.l  a2,-(a7)
                move.w  d2,-(a7)
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                bra.s   postflight_renderer_table_restore
postflight_renderer_table_shared:
                move.w  (a2)+,d1
                move.l  a2,-(a7)
                move.w  d2,-(a7)
                jsr     SUBMIT_SHARED_RENDERER_VALUES.l
postflight_renderer_table_restore:
                move.w  (a7)+,d2
                movea.l (a7)+,a2
                dbra    d2,postflight_renderer_table_loop
postflight_renderer_table_finished:
                moveq   #0,d7
                addq.b  #1,POSTFLIGHT_COUNTER_BYTE.l
                clr.b   POSTFLIGHT_STATUS_BYTE.l
                lea     POSTFLIGHT_TRIPLE_BASE.l,a0
                moveq   #1,d6
                lea     POSTFLIGHT_TABLE_BASE.l,a2
                tst.w   POSTFLIGHT_TABLE_SELECT_WORD.l
                beq.s   postflight_renderer_triple_ready
                ; ADDA.W #$28,A2. Preserve the original immediate form.
                dc.w    $D4FC,$0028
postflight_renderer_triple_ready:
                movem.l (a0)+,d0-d2
                move.l  d0,d3
                or.l    d1,d3
                or.l    d2,d3
                beq.w   POSTFLIGHT_DEEPER_ZERO_TARGET
