; Byte-exact postflight callback prefix $C10DAE-$C10E95.
; Run060 installs this entry at $C1820C in the frame-9284-to-9285 transition.
; The later $C11048 shared epilogue and callback semantics remain external.

                org     $C10DAE

POSTFLIGHT_SAVED_BYTE           equ     $C45798
POSTFLIGHT_RECORD_FLAGS         equ     $C46184
POSTFLIGHT_CONTROL_BYTE         equ     $C46200
POSTFLIGHT_GATE_BYTE            equ     $C4578C
POSTFLIGHT_EXTERNAL_POINTER     equ     $C1AB74
POSTFLIGHT_CONTROL_LATCH        equ     $C457C5
POSTFLIGHT_FLAG_WORD_A          equ     $C458CA
POSTFLIGHT_FLAG_WORD_B          equ     $C458CC
POSTFLIGHT_MODE_BYTE            equ     $C45785
POSTFLIGHT_STATUS_BYTE_A        equ     $C4589A
POSTFLIGHT_STATUS_BYTE_B        equ     $C45899
POSTFLIGHT_WORK_LONG            equ     $C461C6
POSTFLIGHT_CALLBACK_SLOT        equ     $C1820C
POSTFLIGHT_NEXT_CALLBACK        equ     $C11788
POSTFLIGHT_CONTINUE             equ     $C11048

handle_postflight_callback_prefix:
                link.w  a6,#-2
                move.b  POSTFLIGHT_SAVED_BYTE.l,-1(a6)
                move.w  POSTFLIGHT_RECORD_FLAGS.l,d0
                btst    #9,d0
                beq.w   .alternate_state
                move.b  POSTFLIGHT_CONTROL_BYTE.l,d0
                andi.b  #$0F,d0
                tst.b   d0
                bne.w   .alternate_state
                tst.b   POSTFLIGHT_GATE_BYTE.l
                beq.w   .gate_clear
                movea.l POSTFLIGHT_EXTERNAL_POINTER.l,a0
                move.w  $10(a0),d0
                addq.w  #1,d0
                movea.l POSTFLIGHT_EXTERNAL_POINTER.l,a0
                move.w  d0,$10(a0)
                moveq   #1,d0
                move.b  d0,POSTFLIGHT_CONTROL_LATCH.l
                move.w  POSTFLIGHT_FLAG_WORD_A.l,d0
                andi.w  #$FEFF,d0
                move.w  d0,POSTFLIGHT_FLAG_WORD_A.l
                move.w  POSTFLIGHT_FLAG_WORD_B.l,d0
                andi.w  #$FFBF,d0
                move.w  d0,POSTFLIGHT_FLAG_WORD_B.l
                move.w  POSTFLIGHT_RECORD_FLAGS.l,d0
                andi.w  #$FDFF,d0
                move.w  d0,POSTFLIGHT_RECORD_FLAGS.l
                move.b  POSTFLIGHT_MODE_BYTE.l,d0
                tst.b   d0
                bne.b   .install_next_callback
                move.b  #1,POSTFLIGHT_STATUS_BYTE_A.l
                clr.l   POSTFLIGHT_WORK_LONG.l
                move.b  #$28,POSTFLIGHT_STATUS_BYTE_B.l
                jsr     $C0F4A6.l
                moveq   #$78,d0
                move.l  d0,-(sp)
                moveq   #$3F,d0
                move.l  d0,-(sp)
                jsr     $C17F8C.l
                addq.l  #8,sp
.install_next_callback:
                move.l  #POSTFLIGHT_NEXT_CALLBACK,POSTFLIGHT_CALLBACK_SLOT.l
                bra.w   POSTFLIGHT_CONTINUE
.gate_clear:
                move.w  POSTFLIGHT_RECORD_FLAGS.l,d0
                andi.w  #$FDFF,d0
                move.w  d0,POSTFLIGHT_RECORD_FLAGS.l
                move.l  #$4021,-(sp)
                jsr     $C11BB0.l
                addq.l  #4,sp
                bra.w   POSTFLIGHT_CONTINUE
.alternate_state:
