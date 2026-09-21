; Byte-exact observed threshold computation $C2D81A-$C2D839.

                org     $C2D81A

compute_postmatrix_record_threshold:
                subi.w  #$4650,d0
                neg.w   d0
                mulu.w  #$6c,d0
                asr.l   #8,d0
                move.w  $22(a1),d7
                asr.w   #8,d7
                mulu.w  d0,d7
                asr.l   #6,d7
                add.w   d7,d0
                dc.w    $0829,$0007,$0003 ; btst.b #7,3(a1); retain original EA
                bne.b   $C2D844
