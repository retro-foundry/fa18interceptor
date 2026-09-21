; Byte-exact local transform-flag gate $C1F150-$C1F157.
; The taken path starts at the separately unobserved $C1F158.

                org     $C1F150

gate_c1ee14_transform_local_flag:
                btst.b  #0,-127(a6)
                beq.b   $C1F160
