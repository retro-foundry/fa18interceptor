; Byte-exact stream-descriptor cursor publication $C1EEFA-$C1EF15.

                org     $C1EEFA

STREAM_DESCRIPTOR_CURSOR         equ $C45A32
STREAM_STAGE_POINTER             equ $C45A36

load_c1ee14_stream_descriptor:
                move.w  d7,-$62(a6)
                andi.w  #$0FFF,d0
                movea.l -$2C(a6),a3
                lea     $00(a3,d0.w),a1
                move.l  a1,STREAM_DESCRIPTOR_CURSOR.l
                move.l  a2,STREAM_STAGE_POINTER.l
                move.w  (a1)+,d0
                move.b  d0,-$88(a6)
                move.w  (a1)+,d4
                move.w  (a1)+,d5
