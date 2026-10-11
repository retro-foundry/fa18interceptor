; Byte-exact observed C1E540 transform continuation $C1E694-$C1E6A3.
; Bit 4 of the current record word selects one of the paired transform-helper
; sequences before both paths join at $C1E6AC.

                org     $C1E694

select_c1e540_record_transform_helper:
                dc.w    $0801,$0004             ; btst.b #4,d1
                beq.b   $C1E6A4
                bsr.w   $C1EBC0
                bsr.w   $C1EC96
                bra.b   $C1E6AC
