; Byte-exact observed matrix-side record +$6C gate $C13638-$C13647.
; The active record's word +$6C must exceed $0300 to enter the following
; matrix-side bound checks; smaller values use the signed-local guard pair.

                org     $C13638

CURRENT_CONTROL_RECORD          equ     $C18210

gate_matrix_side_record_word6c:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  $6C(a0),d0
                cmpi.w  #$300,d0
                ble.b   $C136A0
