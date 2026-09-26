; Byte-exact observed shifted-component auxiliary-flag gate $C12316-$C1231D.
; A nonzero auxiliary byte takes the flag-clearing path before the component
; midrange classification resumes.

                org     $C12316

gate_shifted_component_auxiliary_flag:
                tst.b   $C45786.l
                beq.b   $C12334
