; Byte-exact observed return tail for the mask-row paths $C3289A-$C328A5.

                org     $C3289A

return_c3289a_mask_rows:
                swap    d2
                move.w  d2,d3
                swap    d6
                ; Captured MOVEM restore mask is intentionally preserved verbatim.
                dc.w    $4CDF,$0043
                rts
