; Byte-exact observed component reconstruction $C274BA-$C274EB.

                org     $C274BA

prepare_c274ba_inner_relative_components:
                move.w  d7,d5
                move.w  d6,d7
                move.w  d0,d6
                ; MOVEM.W (A5),D2-D4; retain observed register-mask words.
                dc.w    $4c95,$001c
                swap    d7
                move.w  -$5a(a6),d7
                asr.w   d7,d2
                asr.l   d7,d3
                asr.w   d7,d4
                swap    d7
                add.w   $c(a3),d2
                add.l   $10(a3),d3
                add.w   $e(a3),d4
                sub.w   a2,d2
                sub.l   a1,d3
                sub.w   -$42(a6),d4
                neg.w   d2
                neg.w   d3
                neg.w   d4
