; Byte-exact observed C13D84 continuation $C147AE-$C147B9.
; It reads the byte through local -$14 and tests bit 0 before the record
; adjustment continuation.

                org     $C147AE

gate_c13d84_record_bit0_adjustment:
                movea.l -$14(a6),a0
                move.b  (a0),d0
                dc.w    $0800,$0000             ; btst.b #0,d0
                beq.b   $C147D0
