; Byte-exact parent update-context initialization $C0F4D8-$C0F569.

                org     $C0F4D8

INITIALIZATION_LABEL             equ $C07FEC
PARENT_UPDATE_CALLBACK           equ $C0F812
PARENT_UPDATE_CALLBACK_SLOT      equ $C1820C
COMMAND_MODE_LATCH               equ $C4584B
INITIAL_CONTROL_LIMIT            equ $03C0
INITIAL_CONTROL_NEGATIVE         equ $FFFFFC40

CALL_C16CD8                      equ $C16CD8
CALL_C16B8C                      equ $C16B8C
CALL_C53B74                      equ $C53B74
CALL_C53B88                      equ $C53B88
CALL_C16D4C                      equ $C16D4C
CALL_C2527C                      equ $C2527C
CALL_C1787A                      equ $C1787A
CALL_C17104                      equ $C17104
CALL_C1712C                      equ $C1712C
CALL_C08EE4                      equ $C08EE4
CALL_C08EB8                      equ $C08EB8

initialize_parent_update_context:
                link.w  a6,#-4
                jsr     CALL_C16CD8.l
                jsr     CALL_C16B8C.l
                pea.l   INITIALIZATION_LABEL.l
                jsr     CALL_C53B74.l
                addq.l  #4,a7
                moveq   #$80,d1
                move.l  d1,-(a7)
                move.l  d0,-(a7)
                move.l  d0,-4(a6)
                jsr     CALL_C53B88.l
                addq.l  #8,a7
                jsr     CALL_C16D4C.l
                jsr     CALL_C2527C.l
                jsr     CALL_C1787A.l
                move.l  #INITIAL_CONTROL_LIMIT,d0
                move.l  d0,-(a7)
                move.l  d0,-(a7)
                move.l  #INITIAL_CONTROL_NEGATIVE,d0
                move.l  d0,-(a7)
                move.l  d0,-(a7)
                jsr     CALL_C17104.l
                lea.l   $10(a7),a7
                moveq   #$64,d0
                move.l  d0,-(a7)
                move.l  #$A0,-(a7)
                jsr     CALL_C1712C.l
                addq.l  #8,a7
                clr.b   COMMAND_MODE_LATCH.l
                jsr     CALL_C08EE4.l
                jsr     CALL_C08EB8.l
                lea.l   PARENT_UPDATE_CALLBACK(pc),a0
                move.l  a0,PARENT_UPDATE_CALLBACK_SLOT.l
                unlk    a6
                rts
