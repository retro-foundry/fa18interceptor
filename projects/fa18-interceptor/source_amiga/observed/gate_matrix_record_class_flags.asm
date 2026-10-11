; Byte-exact observed matrix-record class/flag gate $C2D4B2-$C2D4C5.
; Record bit 0 bypasses this check.  Otherwise byte +$64 is copied to D3 and
; bit 4 selects the shared class continuation; clear reaches the mask route.

                org     $C2D4B2

gate_matrix_record_class_flags:
                btst    #0,$2(a1)
                bne.b   $C2D4D0
                move.b  $64(a1),d1
                move.b  d1,d3
                andi.b  #$10,d3
                bne.b   $C2D4D0
