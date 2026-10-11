; Byte-exact indexed-update upper-bound comparison $C25CCA-$C25CD1.

                org     $C25CCA

compare_indexed_update_upper_bound:
                cmpi.w  #$3840,$6A(a1)
                dc.w    $6E0A                   ; bgt.b $C25CDC
