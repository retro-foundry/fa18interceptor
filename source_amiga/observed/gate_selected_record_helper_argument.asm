; Byte-exact observed selected-record helper guard $C13BA0-$C13BBB.
; The prefix routes to the shared continuation when record bit 7 is set, its
; word +$26 is zero, or the caller's word argument is non-positive.

                org     $C13BA0

CURRENT_CONTROL_RECORD          equ     $C18210

gate_selected_record_helper_argument:
                link.w  a6,#-6
                dc.w    $2079                   ; movea.l $C18210,a0
                dc.l    CURRENT_CONTROL_RECORD
                move.w  2(a0),d0
                btst    #7,d0
                bne.b   $C13BC2
                move.w  $26(a0),d0
                tst.w   d0
                beq.b   $C13BC2
                tst.w   $A(a6)
                ble.b   $C13C06
