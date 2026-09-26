; Byte-exact observed C2C392 dispatch continuation $C2C40E-$C2C41D.
; D0 value eight, or a shared mode byte other than five, sends execution to
; the common dispatch route at $C2C45E.

                org     $C2C40E

MATRIX_DISPATCH_MODE           equ     $C458A6

gate_c2c392_dispatch_mode:
                cmpi.b  #8,d0
                beq.b   $C2C45E
                cmpi.b  #5,MATRIX_DISPATCH_MODE.l
                bne.b   $C2C45E
