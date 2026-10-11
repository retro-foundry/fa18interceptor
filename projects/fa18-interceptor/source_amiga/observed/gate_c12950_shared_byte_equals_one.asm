; Byte-exact observed C12950 control-record continuation $C129F4-$C129FD.
; It reloads the shared byte and branches onward when its value is not one.

                org     $C129F4

CONTROL_RECORD_SIGNED_BYTE      equ     $C45885

gate_c12950_shared_byte_equals_one:
                move.b  CONTROL_RECORD_SIGNED_BYTE.l,d0
                subq.b  #1,d0
                bne.b   $C12A0A
