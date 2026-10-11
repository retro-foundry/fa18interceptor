; Byte-exact observed selected-record byte +$20 bit-2 gate $C13BC2-$C13BD1.
; Bit 2 clear bypasses the caller-word halving path; set falls through to the
; subsequent adjustment of the stack word.

                org     $C13BC2

CURRENT_CONTROL_RECORD          equ     $C18210

gate_selected_record_byte20_bit2:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  $20(a0),d0
                btst    #2,d0
                beq.b   $C13BDC
