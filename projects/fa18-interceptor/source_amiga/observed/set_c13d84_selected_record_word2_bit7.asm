; Byte-exact C13D84 continuation $C14E04-$C14E7D.
; The native run060 frame-8796 continuation takes the entry, ORs bit 7 into
; the selected local record word, and in that trace A0 is $C46186.

                org     $C14E04

CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_WORD_26                  equ     $26
POST_UPDATE_LONG_SOURCE         equ     $C45AF2
POST_UPDATE_LONG_DESTINATION    equ     $C4590C
POST_UPDATE_CLEAR_BYTE          equ     $C4579F
POST_UPDATE_SET_BYTE            equ     $C457C0
POST_UPDATE_STATUS              equ     $C4589A
POST_UPDATE_EXTERNAL_POINTER    equ     $C1AB74
POST_UPDATE_CONTROL_LATCH       equ     $C457C5
POST_UPDATE_SELECTOR            equ     $C45798

set_c13d84_selected_record_word2_bit7:
                ori.w   #$0080,d0
                move.w  d0,(a0)
                movea.l CURRENT_CONTROL_RECORD.l,a0
                clr.w   RECORD_WORD_26(a0)
                jsr     $C16D04.l
                move.l  POST_UPDATE_LONG_SOURCE.l,POST_UPDATE_LONG_DESTINATION.l
                moveq   #0,d0
                move.b  d0,POST_UPDATE_CLEAR_BYTE.l
                moveq   #1,d0
                move.b  d0,POST_UPDATE_SET_BYTE.l
                cmpi.w  #$0240,-4(a6)
                blt.b   .after_threshold
                move.b  POST_UPDATE_STATUS.l,d0
                tst.b   d0
                bne.w   $C15028
                movea.l -$28(a6),a0
                move.w  (a0),d0
                ori.w   #$0200,d0
                move.w  d0,(a0)
                movea.l POST_UPDATE_EXTERNAL_POINTER.l,a0
                move.w  $46(a0),d0
                addq.w  #1,d0
                movea.l POST_UPDATE_EXTERNAL_POINTER.l,a0
                move.w  d0,$46(a0)
                move.b  #1,POST_UPDATE_CONTROL_LATCH.l
                move.b  #4,POST_UPDATE_SELECTOR.l
                bra.w   $C15028
.after_threshold:
                tst.w   $C461F2.l
                beq.w   $C15028
                move.w  $C461F2.l,$C461F0.l
                move.b  #$FE,$C458B0.l
                movea.l -$30(a6),a0
                move.b  (a0),d0
                btst    #1,d0
                bne.b   .check_record_limit
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  $7C(a0),d0
                andi.b  #$70,d0
                tst.b   d0
                beq.b   .post_limit_check
.check_record_limit:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  $6E(a0),d0
                cmpi.w  #$03C0,d0
                blt.b   .post_limit_check
                movea.l -$2C(a6),a0
                move.w  (a0),d0
                ori.w   #$1000,d0
                move.w  d0,(a0)
                move.w  $C459B6.l,$C4FDD2.l
.post_limit_check:
                cmpi.w  #$0180,-4(a6)
                ble.b   .small_delay
                moveq   #$28,d0
                move.l  d0,-(a7)
                jsr     $C1803C.l
                addq.l  #4,a7
                bra.w   $C15028
.small_delay:
                moveq   #$1E,d0
                move.l  d0,-(a7)
                jsr     $C1803C.l
                addq.l  #4,a7
                bra.w   $C15028
