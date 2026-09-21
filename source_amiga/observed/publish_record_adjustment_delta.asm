; Byte-exact observed delta publication $C2DDE8-$C2DDF1.

                org     $C2DDE8

publish_record_adjustment_delta:
                sub.w   d1,d7
                move.w  d7,$54(a1)
                move.w  d7,d4
                bge.b   $C2DE00
