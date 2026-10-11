; Byte-exact postflight message-sequence prefix $C110A4-$C111A9.
; The run060 success activation takes the mode-9 route, writing $004A,$8053.

                org     $C110A4

POSTFLIGHT_DELAY                equ     $C45AD6
POSTFLIGHT_MODE                 equ     $C458A6
MESSAGE_SELECTOR_SEQUENCE       equ     $C4574A
POSTFLIGHT_ACTIVE_FLAG          equ     $C45795
POSTFLIGHT_CALLBACK_SLOT        equ     $C1820C
POSTFLIGHT_NEXT_CALLBACK        equ     $C10DAE
POSTFLIGHT_SELECTOR_STATE       equ     $C45798
POSTFLIGHT_EXTERNAL_POINTER     equ     $C1AB74
POSTFLIGHT_MODE9_FLAG           equ     $C4582B
INITIALIZE_MESSAGE_SEQUENCE     equ     $C11312
POSTFLIGHT_MODE9_HELPER         equ     $C1643A
POSTFLIGHT_MODE_RANGE_HELPER    equ     $C24FA4
POSTFLIGHT_CONTINUE             equ     $C112DC
POSTFLIGHT_RETURN               equ     $C1130E
POSTFLIGHT_OTHER_MODE           equ     $C111AA
POSTFLIGHT_SELECTOR_STATE_OTHER equ     $C111BA

prepare_postflight_message_sequence:
                link.w  a6,#-6
                move.w  POSTFLIGHT_DELAY.l,d0
                tst.w   d0
                bpl.w   POSTFLIGHT_RETURN
                move.b  POSTFLIGHT_MODE.l,d0
                ext.w   d0
                move.l  #MESSAGE_SELECTOR_SEQUENCE,-6(a6)
                clr.b   POSTFLIGHT_ACTIVE_FLAG.l
                lea.l   POSTFLIGHT_NEXT_CALLBACK(pc),a0
                move.l  a0,POSTFLIGHT_CALLBACK_SLOT.l
                move.w  d0,-2(a6)
                bsr.w   INITIALIZE_MESSAGE_SEQUENCE
                move.b  POSTFLIGHT_SELECTOR_STATE.l,d0
                cmpi.b  #$FF,d0
                bne.w   POSTFLIGHT_SELECTOR_STATE_OTHER
                move.w  -2(a6),d0
                cmpi.w  #3,d0
                bcs.b   .test_mode_special
                cmpi.w  #8,d0
                bhi.b   .test_mode_special
                moveq   #0,d1
                move.w  -2(a6),d1
                moveq   #4,d0
                move.l  d0,-(sp)
                clr.l   -(sp)
                move.l  d1,-(sp)
                jsr     POSTFLIGHT_MODE_RANGE_HELPER.l
                lea.l   12(sp),sp
                movea.l -6(a6),a0
                addq.l  #2,a0
                move.l  a0,-6(a6)
                cmpi.w  #3,-2(a6)
                bne.b   .append_range_tail
                move.w  #$8055,(a0)
                addq.l  #2,-6(a6)
                bra.w   POSTFLIGHT_CONTINUE
.append_range_tail:
                movea.l -6(a6),a0
                move.w  #$8056,(a0)
                addq.l  #2,-6(a6)
                bra.w   POSTFLIGHT_CONTINUE
.test_mode_special:
                move.b  POSTFLIGHT_MODE.l,d0
                ext.w   d0
                ext.l   d0
                cmpi.l  #9,d0
                beq.b   .mode9
                cmpi.l  #$7D,d0
                bne.b   POSTFLIGHT_OTHER_MODE
                movea.l -6(a6),a0
                move.w  #$48,(a0)
                addq.l  #2,-6(a6)
                move.b  #$F0,POSTFLIGHT_SELECTOR_STATE.l
                bra.w   POSTFLIGHT_CONTINUE
.mode9:
                movea.l POSTFLIGHT_EXTERNAL_POINTER.l,a0
                move.w  #1,(a0)
                jsr     POSTFLIGHT_MODE9_HELPER.l
                movea.l -6(a6),a0
                move.w  #$4A,(a0)
                addq.l  #2,a0
                move.w  #$8053,(a0)
                addq.l  #2,a0
                move.b  #1,POSTFLIGHT_MODE9_FLAG.l
                move.b  #$EF,POSTFLIGHT_SELECTOR_STATE.l
                move.l  a0,-6(a6)
                bra.w   POSTFLIGHT_CONTINUE
