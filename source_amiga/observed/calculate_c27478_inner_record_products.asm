; Byte-exact observed paired-record differences and fixed-point products
; $C27478-$C274B9.  The shared-frame scale count remains unnamed.

                org     $C27478

calculate_c27478_inner_record_products:
                ; MOVEM.W $0(A3,D3.W),D2-D4; observed indexed EA/mask words.
                dc.w    $4cb3,$001c,$3000
                sub.w   (a5),d2
                sub.w   $2(a5),d3
                sub.w   $4(a5),d4
                ; MOVEM.W $0(A3,D0.W),D5-D7; observed indexed EA/mask words.
                dc.w    $4cb3,$00e0,$0000
                sub.w   (a5),d5
                sub.w   $2(a5),d6
                sub.w   $4(a5),d7
                move.w  d4,d0
                move.w  d7,d1
                muls.w  d3,d7
                muls.w  d6,d4
                sub.l   d4,d7
                moveq   #7,d4
                add.w   -$5a(a6),d4
                asr.l   d4,d7
                muls.w  d5,d0
                muls.w  d2,d1
                sub.l   d1,d0
                asr.l   d4,d0
                muls.w  d2,d6
                muls.w  d3,d5
                sub.l   d5,d6
                asr.l   d4,d6
