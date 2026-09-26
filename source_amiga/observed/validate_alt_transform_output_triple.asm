; Byte-exact alternate-transform output gate $C1F4FE-$C1F51D.
; It rejects the transformed triple unless D4 and the prior first/second output
; words satisfy the signed interval tests, otherwise resumes the next source
; triple at $C1F524.

                org     $C1F4FE

validate_alt_transform_output_triple:
                move.w  d4,d2
                ble.b   .reject
                asr.w   #1,d2
                add.w   d2,d4
                move.w  -$6(a3),d2
                cmp.w   d4,d2
                bgt.b   .reject
                neg.w   d2
                cmp.w   d4,d2
                bgt.b   .reject
                cmp.w   d4,d7
                bgt.b   .reject
                neg.w   d7
                cmp.w   d4,d7
                ble.b   $C1F524
.reject:
