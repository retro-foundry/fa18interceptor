; Byte-exact observed C123FA scaled-component mode gate $C125D6-$C125DD.
; Local mode four selects the additive adjustment path.

                org     $C125D6

gate_c123fa_scaled_component_mode4:
                cmpi.b  #4,-$A(a6)
                bne.b   $C125E8
