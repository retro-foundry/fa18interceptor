; Byte-exact stream vector normalization $C1EF44-$C1EF5D.

                org     $C1EF44

normalize_c1ee14_stream_vector:
                movem.l d0-d2,-$20(a6)
                asr.l   #8,d0
                asr.l   #8,d1
                asr.l   #8,d2
                movem.w d0-d2,-$86(a6)
                neg.w   d0
                neg.w   d1
                neg.w   d2
                movem.w d0-d2,-$26(a6)
