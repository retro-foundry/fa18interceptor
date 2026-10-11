; Byte-exact observed C13D84 selected-record bit-7 gate $C1477C-$C14797.
; It takes the adjacent path unless both the local record word and selected
; record byte +$7C carry bit 7.

                org     $C1477C

CURRENT_CONTROL_RECORD          equ     $C18210

gate_c13d84_selected_record_bit7_path:
                movea.l -$30(a6),a0
                move.w  (a0),d0
                btst    #7,d0
                beq.b   $C147AE
                dc.w    $2079                   ; movea.l $C18210,a0
                dc.l    CURRENT_CONTROL_RECORD
                move.b  $7C(a0),d0
                btst    #7,d0
                beq.b   $C147AE
