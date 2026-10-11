; Byte-exact record-update threshold-selector gates $C23BD2-$C23BF3.

                org     $C23BD2

gate_record_update_threshold_selector:
                dc.w    $0829,$0000,$0002       ; btst.b #0,$02(a1)
                bne.b   $C23BF6
                move.b  $05(a1),d3
                beq.b   $C23BF6
                cmpi.b  #1,d3
                beq.b   $C23BF6
                cmpi.w  #$0360,$4A(a1)
                ble.b   $C23C5C
                cmpi.b  #6,d3
                dc.w    $6718                   ; beq.b $C23C0C
