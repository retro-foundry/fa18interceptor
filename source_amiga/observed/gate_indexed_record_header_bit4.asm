; Byte-exact indexed-record header bit-4 gate $C1EC3A-$C1EC41.
; A set header bit selects the indexed-record byte-origin path.

                org     $C1EC3A

gate_indexed_record_header_bit4:
                move.w  (a1),d1
                dc.w    $0801,$0004             ; btst.b #4,d1
                bne.b   $C1EC58
