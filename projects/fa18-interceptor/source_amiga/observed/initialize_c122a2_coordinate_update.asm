; Byte-exact observed coordinate-update helper boundary $C122A0-$C122AD.
; The leading return closes the prior leaf. The adjacent helper allocates its
; six-byte local frame and tests a shared byte before its update route.

                org     $C122A0

COORDINATE_UPDATE_ENABLE       equ     $C45785

                rts

initialize_c122a2_coordinate_update:
                link.w  a6,#-$6
                tst.b   COORDINATE_UPDATE_ENABLE.l
                beq.b   $C122C0
