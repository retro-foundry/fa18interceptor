; Byte-exact observed scale preparation $C2DE2C-$C2DE55.
; The alternate below-$3840 body at $C2DE56 remains raw.

                org     $C2DE2C

prepare_record_adjustment_scale:
                move.w  d4,d7
                asr.w   #2,d7
                sub.w   d7,d4
                moveq   #13,d7
                ext.l   d3
                ext.l   d5
                asl.l   #7,d3
                asl.l   #7,d5
                neg.l   d3
                neg.l   d5
                asr.l   d7,d3
                asr.l   d7,d5
                muls.w  d4,d3
                muls.w  d4,d5
                asr.l   d7,d3
                asr.l   d7,d5
                move.w  $66(a1),d1
                cmpi.w  #$3840,d1
                blt.b   $C2DE5C
