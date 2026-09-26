; Byte-exact observed C1271C control-continuation setup $C1271C-$C12733.
; It sets the control byte, copies two local longwords into outgoing argument
; slots, and joins the shared continuation at $C12934.

                org     $C1271C

CONTROL_MODE_BYTE               equ     $C457A6

prepare_c1271c_control_continuation:
                move.b  #1,CONTROL_MODE_BYTE
                move.l  -$E(a6),8(a6)
                move.l  -$12(a6),$C(a6)
                bra.w   $C12934
