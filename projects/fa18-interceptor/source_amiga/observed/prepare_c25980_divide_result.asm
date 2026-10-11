; Byte-exact observed divide-result preparation $C25980-$C25993.

                org     $C25980

C25996                         equ     $C25996
C45ACC                         equ     $C45ACC
C45AD0                         equ     $C45AD0

prepare_c25980_divide_result:
                movem.l d0-d2,-(sp)
                move.l  C45ACC.l,d0
                move.w  C45AD0.l,d1
                move.w  d1,d2
                bge.b   C25996
