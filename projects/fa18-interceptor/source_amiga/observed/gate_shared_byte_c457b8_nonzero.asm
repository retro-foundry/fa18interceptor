; Byte-exact observed shared-byte nonzero gate $C12C84-$C12C8B.
; A zero byte bypasses the decrement-and-follow-up path.

                org     $C12C84

gate_shared_byte_c457b8_nonzero:
                tst.b   $C457B8.l
                beq.b   $C12CF0
