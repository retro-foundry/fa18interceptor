; Byte-exact observed shifted-coordinate midrange gate $C12334-$C12343.
; The local word is in the interior range only when greater than $01C2 and
; below $0C80; boundary values take the following clear-state continuation.

                org     $C12334

gate_shifted_coordinate_midrange:
                move.w  -$2(a6),d0
                cmpi.w  #$1C2,d0
                ble.b   $C1234E
                cmpi.w  #$C80,d0
                bge.b   $C1234E
