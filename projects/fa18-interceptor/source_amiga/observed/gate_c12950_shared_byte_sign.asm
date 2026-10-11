; Byte-exact observed C12950 control-record continuation $C129AE-$C129B7.
; It tests the signed shared byte at $C45885 and takes the nonnegative route
; to the later control-record continuation.

                org     $C129AE

CONTROL_RECORD_SIGNED_BYTE      equ     $C45885

gate_c12950_shared_byte_sign:
                move.b  CONTROL_RECORD_SIGNED_BYTE.l,d0
                tst.b   d0
                bpl.b   $C129F4
