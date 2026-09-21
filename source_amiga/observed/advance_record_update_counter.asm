; Byte-exact generic record-update counter/flag gate $C23AA4-$C23AAF.

                org     $C23AA4

advance_record_update_counter:
                subq.w  #1,$4C(a1)
                dc.w    $0829,$0004,$0000       ; btst.b #4,$00(a1)
                dc.w    $67CA                   ; beq.b $C23A7A
