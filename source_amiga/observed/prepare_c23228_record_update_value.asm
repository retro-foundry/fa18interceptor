; Byte-exact observed record-update value preparation $C23228-$C2323B.

                org     $C23228

C2323E                         equ     $C2323E
C45946                         equ     $C45946

prepare_c23228_record_update_value:
                move.w  $56(a1),d0
                move.w  d0,d1
                asr.w   #3,d1
                add.w   d1,d0
                add.w   C45946.l,d0
                asr.w   #3,d0
                bge.b   C2323E
