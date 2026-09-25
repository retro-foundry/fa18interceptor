; Byte-exact activity-input initializer $C16D04-$C16D4B.
; It initializes a C1AB84 request block, calls the external preparation entry,
; and publishes the returned pair to the activity source longwords.

                org     $C16D04

ACTIVITY_REQUEST_BYTE           equ     $C1AB8C
ACTIVITY_REQUEST_CLEAR_BYTE     equ     $C1AB8D
ACTIVITY_REQUEST_PTR_A          equ     $C1AB8E
ACTIVITY_REQUEST_PTR_B          equ     $C1AB92
ACTIVITY_REQUEST_WORD           equ     $C1ABA0
ACTIVITY_REQUEST_BLOCK          equ     $C1AB84
ACTIVITY_REQUEST_RESULT_A       equ     $C1ABA4
ACTIVITY_REQUEST_RESULT_B       equ     $C1ABA8
ACTIVITY_SOURCE_A               equ     $C45AF2
ACTIVITY_SOURCE_B               equ     $C45AF6
EXTERNAL_ACTIVITY_PREPARE       equ     $C53C78

initialize_c16d04_activity_inputs:
                move.b  #5,ACTIVITY_REQUEST_BYTE.l
                clr.b   ACTIVITY_REQUEST_CLEAR_BYTE.l
                suba.l  a0,a0
                move.l  a0,ACTIVITY_REQUEST_PTR_A.l
                move.l  a0,ACTIVITY_REQUEST_PTR_B.l
                move.w  #$A,ACTIVITY_REQUEST_WORD.l
                pea     ACTIVITY_REQUEST_BLOCK.l
                jsr     EXTERNAL_ACTIVITY_PREPARE.l
                addq.l  #4,sp
                move.l  ACTIVITY_REQUEST_RESULT_A.l,ACTIVITY_SOURCE_A.l
                move.l  ACTIVITY_REQUEST_RESULT_B.l,ACTIVITY_SOURCE_B.l
                rts
