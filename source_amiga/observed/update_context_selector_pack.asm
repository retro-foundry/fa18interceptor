; Byte-exact context-selector pack update $C1C6BC-$C1C7F5.
; This is a dataflow reconstruction.  It chooses record- or origin-derived
; input words, updates the pack at C4594C/C45850, and ORs a change into C45858.

                org     $C1C6BC

CONTEXT_SELECTOR_GUARD          equ     $C45785
ACTIVE_RECORD_BASE              equ     $C46184
ACTIVE_RECORD_OFFSET             equ     $C458DE
ACTIVE_RECORD_PACK_X             equ     $06
ACTIVE_RECORD_PACK_Y             equ     $08
ACTIVE_RECORD_PACK_BYTE          equ     $0A
ORIGIN_COMPONENT_X               equ     $C45C3E
ORIGIN_COMPONENT_Z               equ     $C45C46
SELECTOR_PACK_WORD_X             equ     $C4594C
SELECTOR_PACK_WORD_Y             equ     $C4594E
SELECTOR_PACK_BYTE_X             equ     $C45850
SELECTOR_PACK_BYTE_Y             equ     $C45851
SELECTOR_CHANGE_BYTE             equ     $C45858
SELECTOR_MODE                    equ     $C458AE
SELECTOR_MODE_TWO                equ     2
SELECTOR_DEPTH_COMPONENT         equ     $C45A78
SELECTOR_DEPTH_LIMIT             equ     $FFFFF000
ACTIVE_ORIGIN_UPDATE             equ     $C29042
PACK_LIMIT_HELPER                equ     $C1C7F6
ORIGIN_COMPONENT_MASK            equ     $1FFFFFFF

update_context_selector_pack:
                tst.b   CONTEXT_SELECTOR_GUARD.l
                bne.b   update_context_selector_pack_from_origin
                lea     ACTIVE_RECORD_BASE.l,a3
                dc.w    $D6F9,$00C4,$58DE     ; adda.w ACTIVE_RECORD_OFFSET.l,a3
                bsr.w   PACK_LIMIT_HELPER
                move.w  ACTIVE_RECORD_PACK_X(a3),SELECTOR_PACK_WORD_X.l
                move.w  ACTIVE_RECORD_PACK_Y(a3),SELECTOR_PACK_WORD_Y.l
                move.b  ACTIVE_RECORD_PACK_BYTE(a3),SELECTOR_PACK_BYTE_Y.l
                move.w  SELECTOR_PACK_WORD_X.l,d0
                andi.w  #3,d0
                subq.w  #3,d0
                neg.w   d0
                move.w  SELECTOR_PACK_WORD_Y.l,d1
                andi.w  #3,d1
                subq.w  #3,d1
                neg.w   d1
                asl.w   #2,d1
                add.w   d0,d1
                move.b  d1,SELECTOR_PACK_BYTE_X.l
                bra.w   update_context_selector_pack_finish
update_context_selector_pack_from_origin:
                move.w  d5,-(a7)
                jsr     ACTIVE_ORIGIN_UPDATE.l
                move.w  (a7)+,d5
                lea     ACTIVE_RECORD_BASE.l,a3
                bsr.w   PACK_LIMIT_HELPER
                move.l  ORIGIN_COMPONENT_X.l,d0
                move.l  ORIGIN_COMPONENT_Z.l,d1
                andi.l  #ORIGIN_COMPONENT_MASK,d0
                andi.l  #ORIGIN_COMPONENT_MASK,d1
                swap    d0
                swap    d1
                asr.w   #4,d0
                asr.w   #4,d1
                move.w  d0,d2
                move.w  d1,d3
                andi.w  #3,d0
                andi.w  #3,d1
                subq.b  #3,d0
                neg.b   d0
                subq.b  #3,d1
                neg.b   d1
                add.b   d1,d1
                add.b   d1,d1
                add.b   d0,d1
                asr.w   #2,d2
                asr.w   #2,d3
                move.w  d2,d0
                move.w  d3,d4
                andi.w  #3,d2
                andi.w  #3,d3
                subq.b  #3,d2
                neg.b   d2
                subq.b  #3,d3
                neg.b   d3
                add.b   d3,d3
                add.b   d3,d3
                add.b   d2,d3
                cmp.b   SELECTOR_PACK_BYTE_Y.l,d1
                beq.b   update_context_selector_pack_byte_y_done
                cmpi.b  #SELECTOR_MODE_TWO,SELECTOR_MODE.l
                bne.b   update_context_selector_pack_byte_y_changed
                cmpi.l  #SELECTOR_DEPTH_LIMIT,SELECTOR_DEPTH_COMPONENT.l
                ble.b   update_context_selector_pack_byte_y_store
update_context_selector_pack_byte_y_changed:
                moveq   #-1,d5
update_context_selector_pack_byte_y_store:
                move.b  d1,SELECTOR_PACK_BYTE_Y.l
update_context_selector_pack_byte_y_done:
                cmp.b   SELECTOR_PACK_BYTE_X.l,d3
                beq.b   update_context_selector_pack_byte_x_done
                cmpi.b  #SELECTOR_MODE_TWO,SELECTOR_MODE.l
                bne.b   update_context_selector_pack_byte_x_changed
                cmpi.l  #SELECTOR_DEPTH_LIMIT,SELECTOR_DEPTH_COMPONENT.l
                ble.b   update_context_selector_pack_byte_x_store
update_context_selector_pack_byte_x_changed:
                moveq   #-1,d5
update_context_selector_pack_byte_x_store:
                move.b  d3,SELECTOR_PACK_BYTE_X.l
update_context_selector_pack_byte_x_done:
                cmp.w   SELECTOR_PACK_WORD_X.l,d0
                beq.b   update_context_selector_pack_word_x_done
                moveq   #-1,d5
                move.w  d0,SELECTOR_PACK_WORD_X.l
update_context_selector_pack_word_x_done:
                cmp.w   SELECTOR_PACK_WORD_Y.l,d4
                beq.b   update_context_selector_pack_finish
                moveq   #-1,d5
                move.w  d4,SELECTOR_PACK_WORD_Y.l
update_context_selector_pack_finish:
                or.b    d5,SELECTOR_CHANGE_BYTE.l
                rts
