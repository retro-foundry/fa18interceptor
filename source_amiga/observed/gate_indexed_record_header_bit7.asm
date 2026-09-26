; Byte-exact indexed-record header bit-7 gate $C25D94-$C25D9B.

                org     $C25D94

gate_indexed_record_header_bit7:
                dc.w    $0829,$0007,$0000       ; btst.b #7,$0(a1)
                bne.b   $C25DA6
