; Byte-exact indexed-update lower-bound comparison $C25CD2-$C25CDB.

                org     $C25CD2

compare_indexed_update_lower_bound:
                cmpi.w  #$04B0,$6A(a1)
                dc.w    $6D48                   ; blt.b $C25D22
