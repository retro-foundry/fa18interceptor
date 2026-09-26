; Byte-exact observed indexed-update continuation $C25FA2-$C25FAB.
; It clears bit 7 from D1's packed byte and routes values at least $60 to the
; shared indexed-update continuation.

                org     $C25FA2

gate_indexed_update_packed_byte_limit:
                dc.w    $0881,$0007             ; bclr.b #7,d1
                cmpi.b  #$60,d1
                bge.b   $C25FC2
