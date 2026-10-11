; Byte-exact observed signed-byte fixed-point expansion $C275A4-$C275B7.

                org     $C275A4

expand_c275a4_candidate_deltas:
                ext.w   d0
                swap    d0
                asr.l   #2,d0
                ext.w   d1
                swap    d1
                asr.l   #2,d1
                move.l  d0,-$38(a6)
                move.l  d1,-$3c(a6)
