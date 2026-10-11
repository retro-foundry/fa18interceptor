; Byte-exact observed C13D84 continuation $C14456-$C14461.
; It reads the byte through local -$14 and tests bit 0 before the adjacent
; local-state continuation.

                org     $C14456

gate_c13d84_secondary_local_bit0:
                movea.l -$14(a6),a0
                move.b  (a0),d0
                dc.w    $0800,$0000             ; btst.b #0,d0
                beq.b   $C14466
