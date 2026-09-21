; Byte-exact static-only structural/dataflow routine $C30918-$C309A1.

                org     $C30918

SEGMENT37_HELPER_RETURN         equ $C30916
SUBMIT_ADJACENT_RENDERER_VALUES equ $C2F60A
ROW_LOOP_SOURCE_WORD             equ $C458F6
ROW_LOOP_MODE_BYTE               equ $C45837
ROW_LOOP_CACHE_WORD              equ $C459A4
ROW_LOOP_ENABLE_BYTE             equ $C458DB
ROW_LOOP_HORIZONTAL_OFFSET       equ $C45988
ROW_LOOP_VERTICAL_OFFSET         equ $C458D8
ROW_LOOP_STORE_WORD              equ $C45954

run_segment37_renderer_row_loop:
                move.w  ROW_LOOP_SOURCE_WORD.l,d2
                andi.w  #$7FFF,d2
                lsr.w   #8,d2
                lsr.w   #2,d2
                tst.b   ROW_LOOP_MODE_BYTE.l
                bgt.s   segment37_row_loop_store_cache
                cmp.w   ROW_LOOP_CACHE_WORD.l,d2
                beq.s   SEGMENT37_HELPER_RETURN
                btst    #0,ROW_LOOP_ENABLE_BYTE.l
                beq.s   segment37_row_loop_prepare
segment37_row_loop_store_cache:
                move.w  d2,ROW_LOOP_CACHE_WORD.l
segment37_row_loop_prepare:
                move.w  #$15,d3
                move.w  #$70,d0
                add.w   ROW_LOOP_HORIZONTAL_OFFSET.l,d0
                ble.s   SEGMENT37_HELPER_RETURN
                cmpi.w  #$13C,d0
                bgt.s   SEGMENT37_HELPER_RETURN
                move.w  #$B4,d1
                add.w   ROW_LOOP_VERTICAL_OFFSET.l,d1
segment37_row_loop_next:
                subq.w  #1,d2
                bgt.s   segment37_row_loop_select_store
                move.w  #0,ROW_LOOP_STORE_WORD.l
                bra.s   segment37_row_loop_submit
segment37_row_loop_select_store:
                move.w  #4,ROW_LOOP_STORE_WORD.l
segment37_row_loop_submit:
                movem.w d0-d3,-(a7)
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                movem.w (a7)+,d0-d3
                movem.w d0-d3,-(a7)
                addq.w  #2,d0
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                movem.w (a7)+,d0-d3
                subq.w  #1,d1
                dbra    d3,segment37_row_loop_next
                rts
