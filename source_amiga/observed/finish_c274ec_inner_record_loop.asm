; Byte-exact observed inner-loop tail $C274EC-$C27503.

                org     $C274EC

INNER_RECORD_NEXT               equ     $C27456

finish_c274ec_inner_record_loop:
                muls.w  d2,d5
                muls.w  d3,d6
                muls.w  d4,d7
                add.l   d5,d7
                add.l   d6,d7
                blt.w   INNER_RECORD_NEXT
.advance_stream:
                tst.l   (a4)+
                bge.b   .advance_stream
                subq.w  #2,a4
                moveq   #0,d7
                rts
