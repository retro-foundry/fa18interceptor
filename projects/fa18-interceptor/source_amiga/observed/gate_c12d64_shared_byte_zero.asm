; Byte-exact observed C12D64 record-local continuation $C12DC4-$C12DCD.
; It tests the shared byte at $C45885 and takes the zero-value route to the
; later local-term continuation.

                org     $C12DC4

C12D64_SHARED_BYTE              equ     $C45885

gate_c12d64_shared_byte_zero:
                dc.w    $4a39,$00c4,$5785        ; tst.b $C45885.l
                dc.w    $6700,$016c             ; beq.w $C12F38
