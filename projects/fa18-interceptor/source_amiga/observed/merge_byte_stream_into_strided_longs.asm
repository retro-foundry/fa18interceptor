; Byte-exact runtime-backed loop body $C32858-$C3287D.
; Its following branch/epilogue is outside this observed loop slice.

                org     $C32858

merge_byte_stream_into_strided_longs:
                move.l  #$E0000000,d1
                moveq   #0,d0
                move.b  (a0),d0
                ror.l   #8,d0
                lsr.l   d2,d1
                lsr.l   d2,d0
                move.l  (a3),d3
                and.l   d1,d0
                not.l   d1
                and.l   d3,d1
                or.l    d0,d1
                move.l  d1,(a3)
                addq.w  #1,a0
                dc.w    $D6FC,$0028 ; adda.w #$28,a3; retain original non-relaxed opcode
                dbra    d6,merge_byte_stream_into_strided_longs
