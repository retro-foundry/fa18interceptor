; Byte-exact message-layout state bridge $C32E2E-$C32E5B.
; Branch destinations are retained as raw external message-sequence entries.

                org     $C32E2E

MESSAGE_LAYOUT_STATE_B           equ     $C45746
MESSAGE_LAYOUT_STATE_C           equ     $C45748
MESSAGE_RECORD_ATTRIBUTE         equ     $C457DC
MESSAGE_LAYOUT_TRIPLET           equ     $C4570A
MESSAGE_SEQUENCE_STATE_B_RETURN  equ     $C32CEE
MESSAGE_SEQUENCE_FLAGGED_PATH    equ     $C32E4E
MESSAGE_SEQUENCE_DIRECT_PATH     equ     $C32EFC
MESSAGE_SEQUENCE_STATE_C_RETURN  equ     $C32EFE

advance_message_layout_state_guards:
                subq.w  #1,MESSAGE_LAYOUT_STATE_B.l
                ; BGT.W $C32CEE; retain original word-branch encoding.
                dc.w    $6E00,$FEB6
                ; BTST #0,$C457DC.L; retain original absolute encoding.
                dc.b    8,57,0,0,0,196,87,220
                bne.b   MESSAGE_SEQUENCE_FLAGGED_PATH
                movem.l MESSAGE_LAYOUT_TRIPLET.l,a1-a2/a4
                ; BRA.W $C32EFC; retain original word-branch encoding.
                dc.w    $6000,$00AA
.state_c_guard:
                subq.w  #1,MESSAGE_LAYOUT_STATE_C.l
                ; BLE.W $C32EFC; retain original word-branch encoding.
                dc.w    $6F00,$00A0
