; Byte-exact second-pass initialization $C209DA-$C209E1.
; The unobserved loop-back body begins at $C209E2.

                org     $C209DA

initialize_c209da_geometry_second_pass:
                dc.w    $4284                   ; clr.l   d4
                dc.w    $4285                   ; clr.l   d5
                dc.w    $4286                   ; clr.l   d6
                bra.b   $C209F4
