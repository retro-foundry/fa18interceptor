; Byte-exact observed C13D84 local-delta gate $C143BE-$C143DD.
; It compares signed local -$1C against local -$16 plus $3C; only the excess
; path continues to the subsequent $1D4C bound check.

                org     $C143BE

gate_c13d84_local_delta_upper_range:
                move.w  -$16(a6),d0
                ext.l   d0
                addi.l  #$3C,d0
                move.w  -$1c(a6),d1
                ext.l   d1
                cmp.l   d0,d1
                ble.w   $C146C2
                cmpi.l  #$1D4C,d1
                ble.b   $C143E8
