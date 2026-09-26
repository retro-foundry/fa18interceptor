; Byte-exact observed C131BE continuation $C13204-$C1320D.
; It tests the shared byte at $C45885 and takes the zero-value route to the
; later scale-byte gate.

                org     $C13204

gate_c131be_shared_byte_zero:
                dc.w    $4a39,$00c4,$5785        ; tst.b $C45885.l
                dc.w    $6700,$014c             ; beq.w $C13358
