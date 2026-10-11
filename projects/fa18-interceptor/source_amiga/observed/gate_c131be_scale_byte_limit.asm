; Byte-exact observed C131BE helper continuation $C13358-$C13363.
; A shared byte value at most five bypasses the local scale-halving path;
; larger values proceed to that adjacent adjustment.

                org     $C13358

CONTROL_SCALE_BYTE              equ     $C45889

gate_c131be_scale_byte_limit:
                move.b  CONTROL_SCALE_BYTE.l,d0
                cmpi.b  #5,d0
                ble.b   $C1338C
