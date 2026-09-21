; Byte-exact record-walker setup $C1F6F8-$C1F79F.
; Loads the current control-stream pointer then selects local record handlers.

LINE_EMITTER_MASK              equ     $C456E6
LINE_EMITTER_MASK_NEXT         equ     $C456EA
CONTROL_STREAM_POINTER         equ     $C45A36
RECORD_A1_COUNT_LOCAL          equ     -$66
RECORD_STATUS_LOCAL            equ     -$7C
CONTROL_STREAM_BASE_LOCAL      equ     -$2C
OFFSET_VERTEX_TABLE            equ     $C48390
TRIPLE_WORKSPACE               equ     $C4BF94
SELECT_RECORD_STREAMS          equ     $C1F7A0
RECORD_WALKER_LOOP             equ     $C1F716
RECORD_WALKER_RETURN_ZERO      equ     $C1F79A
RECORD_HANDLER_TRIPLE          equ     $C1FB8C
RECORD_HANDLER_HEX             equ     $C1FB9C
RECORD_HANDLER_OTHER           equ     $C1FC42

                org     $C1F6F8

initialize_record_walker_from_pointer:
                move.l  #$000FFFFF,LINE_EMITTER_MASK.l
                clr.l   LINE_EMITTER_MASK_NEXT.l
                movea.l CONTROL_STREAM_POINTER.l,a5
                clr.w   RECORD_A1_COUNT_LOCAL(a6)
                clr.w   RECORD_STATUS_LOCAL(a6)
record_walker_loop:
                tst.w   RECORD_A1_COUNT_LOCAL(a6)
                bne.w   $C1F844
                move.w  (a5)+,d0
                blt.w   SELECT_RECORD_STREAMS
                move.w  (a5)+,d7
                move.w  d7,d1
                andi.w  #$FC00,d1
                cmpi.w  #$FC00,d1
                beq.s   record_walker_relative_jump
                andi.w  #$0C00,d1
                bne.s   record_walker_call_other
                lea.l   OFFSET_VERTEX_TABLE.l,a3
                move.w  d7,d1
                andi.w  #$2000,d1
                bne.s   record_walker_call_hex
                lea.l   TRIPLE_WORKSPACE.l,a0
                lea.l   (a3,d0.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                movem.w (a5)+,d1-d2
                lea.l   (a3,d1.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                lea.l   (a3,d2.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)
                movea.w d7,a3
                bsr.w   RECORD_HANDLER_TRIPLE
                bra.s   record_walker_handle_result
record_walker_call_hex:
                movem.w (a5)+,d0-d5
                jsr     RECORD_HANDLER_HEX.l
                bra.s   record_walker_handle_result
record_walker_call_other:
                bsr.w   RECORD_HANDLER_OTHER
record_walker_handle_result:
                beq.s   record_walker_relative_jump
                movea.l CONTROL_STREAM_BASE_LOCAL(a6),a3
                adda.w  $2(a5),a3
                lea.l   (a3),a5
                bra.s   record_walker_loop
record_walker_relative_jump:
                movea.l CONTROL_STREAM_BASE_LOCAL(a6),a3
                adda.w  (a5),a3
                lea.l   (a3),a5
                bra.w   record_walker_loop
record_walker_return_zero:
                unlk    a6
                moveq   #0,d0
                rts
