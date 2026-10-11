; Byte-exact observed C13D84 continuation $C13F3C-$C13F47.
; It stores the observed maximum byte $78 through the local -$4 record
; pointer, then joins the common bounded-byte continuation.

                org     $C13F3C

set_c13d84_bounded_byte_maximum:
                movea.l -$4(a6),a0
                move.b  #$78,(a0)
                bra.w   $C14008
