; Byte-exact non-mode-eight record-update continuation $C23D3C-$C23D45.

                org     $C23D3C

continue_nonmodeeight_record_update:
                cmpi.b  #8,$05(a1)
                beq.w   $C241A6
                dc.w    $3029,$0000             ; move.w $00(a1),d0
