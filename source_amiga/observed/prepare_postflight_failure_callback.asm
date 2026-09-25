; Byte-exact postflight failure-callback preparation $C11788-$C1182F.
; Run062 reaches this entry before it installs $C118A0.

                org     $C11788

POSTFLIGHT_CONTEXT_MODE         equ     $C45785
POSTFLIGHT_STATUS_BYTE          equ     $C45899
POSTFLIGHT_RECORD_FLAGS         equ     $C46184
POSTFLIGHT_FLAG_WORD            equ     $C458CC
POSTFLIGHT_STATUS_WORD          equ     $C458D4
POSTFLIGHT_COUNTDOWN_BYTE       equ     $C45897
POSTFLIGHT_MODE_BYTE            equ     $C458A1
POSTFLIGHT_AUXILIARY_BYTE       equ     $C45889
POSTFLIGHT_EVENT_FLAGS          equ     $C458CC
POSTFLIGHT_COUNTDOWN            equ     $C45AD6
POSTFLIGHT_ACTIVE_FLAG          equ     $C45795
POSTFLIGHT_CALLBACK_SLOT        equ     $C1820C
POSTFLIGHT_CONTEXT_HELPER       equ     $C092A0
CLEAR_POSTFLIGHT_WORK_BUFFERS   equ     $C2FD22
FAILURE_MESSAGE_CALLBACK        equ     $C118A0
POSTFLIGHT_PREPARE_FOLLOWUP     equ     $C11830

prepare_postflight_failure_callback:
                move.b  POSTFLIGHT_CONTEXT_MODE.l,d0
                tst.b   d0
                bne.b   .test_context_mode
                move.b  POSTFLIGHT_STATUS_BYTE.l,d0
                tst.b   d0
                beq.b   .clear_postflight_flags
.test_context_mode:
                tst.b   POSTFLIGHT_CONTEXT_MODE.l
                beq.w   .return
                move.w  POSTFLIGHT_RECORD_FLAGS.l,d0
                btst    #10,d0
                bne.b   .return
.clear_postflight_flags:
                move.w  POSTFLIGHT_FLAG_WORD.l,d0
                andi.w  #$9FFF,d0
                move.w  d0,POSTFLIGHT_FLAG_WORD.l
                andi.w  #$FFFE,d0
                move.w  d0,POSTFLIGHT_FLAG_WORD.l
                jsr     POSTFLIGHT_CONTEXT_HELPER.l
                move.b  #$0F,POSTFLIGHT_MODE_BYTE.l
                moveq   #0,d0
                move.b  d0,POSTFLIGHT_AUXILIARY_BYTE.l
                move.w  POSTFLIGHT_STATUS_WORD.l,d0
                andi.w  #$FBFF,d0
                move.w  d0,POSTFLIGHT_STATUS_WORD.l
                move.b  POSTFLIGHT_COUNTDOWN_BYTE.l,d0
                subq.b  #1,d0
                move.b  d0,POSTFLIGHT_COUNTDOWN_BYTE.l
                tst.b   d0
                bgt.b   .install_followup
                jsr     CLEAR_POSTFLIGHT_WORK_BUFFERS.l
                move.w  #5,POSTFLIGHT_COUNTDOWN.l
                clr.b   POSTFLIGHT_ACTIVE_FLAG.l
                lea.l   FAILURE_MESSAGE_CALLBACK(pc),a0
                move.l  a0,POSTFLIGHT_CALLBACK_SLOT.l
                bra.b   .return
.install_followup:
                lea.l   POSTFLIGHT_PREPARE_FOLLOWUP(pc),a0
                move.l  a0,POSTFLIGHT_CALLBACK_SLOT.l
.return:
                rts
