; Byte-exact observed delta setup $C2DDC0-$C2DDC9.

                org     $C2DDC0

prepare_record_adjustment_delta:
                move.w  $54(a1),d1
                move.w  d1,d7
                tst.w   d5
                bge.b   $C2DDCC
