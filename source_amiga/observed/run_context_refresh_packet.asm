; Byte-exact context-refresh packet $C1C860-$C1CA2D.
; Runtime evidence ties its C1D10C calls to scene-template selection.  This
; source retains neutral names for the remaining request bits and callbacks.

                org     $C1C860

REFRESH_GUARD_LONG               equ     $C45A66
REFRESH_GUARD_LIMIT              equ     $F8000000
REFRESH_GUARD_RETURN             equ     $C1C85E
FRAME_LOCAL_ENABLE               equ     -$2C
REFRESH_REQUEST_BITS             equ     $C45858
REFRESH_PREPARED_FLAG            equ     $C45859
REFRESH_CALLBACK_A_FLAG          equ     $C45864
REFRESH_CALLBACK_B_FLAG          equ     $C45865
REFRESH_CONTEXT_GUARD            equ     $C45785
REFRESH_ACTIVE_RECORD_BASE       equ     $C46184
REFRESH_ACTIVE_RECORD_OFFSET     equ     $C458DE
REFRESH_ORIGIN_X                 equ     $C45C3E
REFRESH_ORIGIN_Z                 equ     $C45C46
REFRESH_ORIGIN_MASK              equ     $1FFFFFFF
REFRESH_SELECTOR_X               equ     $C45948
REFRESH_SELECTOR_Z               equ     $C4594A
REFRESH_TRACE_WORD               equ     $C45AD4
REFRESH_ERROR_WORD               equ     $C4599E
REFRESH_ERROR_HELPER             equ     $C06C02
REFRESH_CLASS_HELPER             equ     $C1CA82
REFRESH_SCENE_SELECTOR           equ     $C1D10C
REFRESH_STAGE_A                  equ     $C1E328
REFRESH_STAGE_B                  equ     $C1E540
REFRESH_STAGE_C_CLEAR            equ     $C09A78
REFRESH_STAGE_C_SET              equ     $C09A98
REFRESH_STAGE_C_SELECTOR         equ     $C458DA
REFRESH_RENDER_GUARD_A           equ     $C457A7
REFRESH_RENDER_GUARD_B           equ     $C457A4
REFRESH_RENDER_VALUE_A           equ     $72
REFRESH_RENDER_VALUE_B           equ     $A0
REFRESH_RENDER_FLAG              equ     $C456E6
REFRESH_RENDER_SELECTOR          equ     $C45954
REFRESH_RENDER_SELECTOR_A        equ     $0F
REFRESH_RENDER_SELECTOR_B        equ     $06
REFRESH_RENDER_SUBMIT            equ     $C2F66E

run_context_refresh_packet:
                cmpi.l  #REFRESH_GUARD_LIMIT,REFRESH_GUARD_LONG.l
                blt.b   REFRESH_GUARD_RETURN
                movem.l d0-d5/a0,-(a7)
                clr.b   FRAME_LOCAL_ENABLE(a6)
                move.b  REFRESH_REQUEST_BITS.l,d0
                beq.w   run_context_refresh_packet_after_requests
                move.w  #$49,REFRESH_TRACE_WORD.l
                bsr.w   REFRESH_CLASS_HELPER
                move.b  d0,d1
                andi.b  #$0F,d1
                cmpi.b  #$0C,d1
                beq.b   run_context_refresh_packet_selectors
                cmpi.b  #$0B,d1
                beq.b   run_context_refresh_packet_selectors
                cmpi.b  #$0F,d1
                beq.b   run_context_refresh_packet_selectors
                move.w  #$27,REFRESH_ERROR_WORD.l
                jsr     REFRESH_ERROR_HELPER.l
run_context_refresh_packet_selectors:
                tst.b   REFRESH_CONTEXT_GUARD.l
                beq.b   run_context_refresh_packet_record_selectors
                move.l  REFRESH_ORIGIN_X.l,d1
                move.l  REFRESH_ORIGIN_Z.l,d2
                andi.l  #REFRESH_ORIGIN_MASK,d1
                andi.l  #REFRESH_ORIGIN_MASK,d2
                swap    d1
                swap    d2
                asr.w   #8,d1
                asr.w   #8,d2
                bra.b   run_context_refresh_packet_publish_selectors
run_context_refresh_packet_record_selectors:
                lea     REFRESH_ACTIVE_RECORD_BASE.l,a1
                dc.w    $D2F9,$00C4,$58DE     ; adda.w REFRESH_ACTIVE_RECORD_OFFSET.l,a1
                move.w  $06(a1),d1
                move.w  $08(a1),d2
                asr.w   #2,d1
                asr.w   #2,d2
run_context_refresh_packet_publish_selectors:
                move.w  d1,REFRESH_SELECTOR_X.l
                move.w  d2,REFRESH_SELECTOR_Z.l
                move.b  #1,REFRESH_PREPARED_FLAG.l
                clr.b   REFRESH_CALLBACK_A_FLAG.l
                clr.b   REFRESH_CALLBACK_B_FLAG.l
                btst    #0,d0
                beq.b   run_context_refresh_packet_request_one
                dc.w    $08B9,$0000,$00C4,$5858 ; bclr.b #0,REFRESH_REQUEST_BITS.l
                bsr.w   REFRESH_SCENE_SELECTOR
                dc.w    $0839,$0007,$00C4,$5858 ; btst.b #7,REFRESH_REQUEST_BITS.l
run_context_refresh_packet_request_one:
                dc.w    $0839,$0001,$00C4,$5858 ; btst.b #1,REFRESH_REQUEST_BITS.l
                beq.b   run_context_refresh_packet_request_two
                dc.w    $08B9,$0001,$00C4,$5858 ; bclr.b #1,REFRESH_REQUEST_BITS.l
                move.b  #1,REFRESH_CALLBACK_B_FLAG.l
                bsr.w   REFRESH_SCENE_SELECTOR
                dc.w    $0839,$0007,$00C4,$5858 ; btst.b #7,REFRESH_REQUEST_BITS.l
run_context_refresh_packet_request_two:
                move.b  #1,REFRESH_CALLBACK_A_FLAG.l
                dc.w    $0839,$0002,$00C4,$5858 ; btst.b #2,REFRESH_REQUEST_BITS.l
                beq.b   run_context_refresh_packet_request_three
                dc.w    $08B9,$0002,$00C4,$5858 ; bclr.b #2,REFRESH_REQUEST_BITS.l
run_context_refresh_packet_request_three:
                dc.w    $0839,$0003,$00C4,$5858 ; btst.b #3,REFRESH_REQUEST_BITS.l
                beq.b   run_context_refresh_packet_requests_done
                dc.w    $08B9,$0003,$00C4,$5858 ; bclr.b #3,REFRESH_REQUEST_BITS.l
                bsr.w   REFRESH_SCENE_SELECTOR
                move.w  #$4A,REFRESH_TRACE_WORD.l
run_context_refresh_packet_requests_done:
                move.b  #1,FRAME_LOCAL_ENABLE(a6)
                clr.b   REFRESH_REQUEST_BITS.l
run_context_refresh_packet_after_requests:
                jsr     REFRESH_STAGE_A.l
                move.w  #$4B,REFRESH_TRACE_WORD.l
                clr.b   FRAME_LOCAL_ENABLE(a6)
                jsr     REFRESH_STAGE_B.l
                move.w  #$4C,REFRESH_TRACE_WORD.l
                move.w  REFRESH_STAGE_C_SELECTOR.l,d1
                andi.w  #1,d1
                bne.b   run_context_refresh_packet_stage_c_set
                jsr     REFRESH_STAGE_C_CLEAR.l
                move.w  #$4D,REFRESH_TRACE_WORD.l
                bra.b   run_context_refresh_packet_stage_c_done
run_context_refresh_packet_stage_c_set:
                jsr     REFRESH_STAGE_C_SET.l
                move.w  #$4E,REFRESH_TRACE_WORD.l
run_context_refresh_packet_stage_c_done:
                movem.l (a7)+,d0-d5/a0
                tst.b   REFRESH_RENDER_GUARD_A.l
                bne.b   run_context_refresh_packet_render_done
                tst.b   REFRESH_RENDER_GUARD_B.l
                beq.b   run_context_refresh_packet_render_done
                move.w  #REFRESH_RENDER_VALUE_A,d0
                move.w  #REFRESH_RENDER_VALUE_B,d1
                tst.b   REFRESH_PREPARED_FLAG.l
                beq.b   run_context_refresh_packet_render_selector_b
                move.l  #$000FFFFF,REFRESH_RENDER_FLAG.l
                move.w  #REFRESH_RENDER_SELECTOR_A,REFRESH_RENDER_SELECTOR.l
                bra.b   run_context_refresh_packet_submit_render
run_context_refresh_packet_render_selector_b:
                move.w  #REFRESH_RENDER_SELECTOR_B,REFRESH_RENDER_SELECTOR.l
run_context_refresh_packet_submit_render:
                jsr     REFRESH_RENDER_SUBMIT.l
run_context_refresh_packet_render_done:
                clr.b   REFRESH_PREPARED_FLAG.l
                rts
