; Byte-exact observed tail-helper auxiliary-mode lower-bound gate
; $C2B3CC-$C2B3D5. Values below five return to the helper loop.

                org     $C2B3CC

check_tail_auxiliary_mode_lower_bound:
                cmpi.b  #5,$C458AE.l
                blt.b   $C2B3C0
