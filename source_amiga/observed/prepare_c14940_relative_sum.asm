; Byte-exact observed basic block $C14940-$C1495B.

                org     $C14940

ACTIVE_RECORD_POINTER           equ     $C18210

prepare_c14940_relative_sum:
                move.w  -$4(a6),d0
                asr.w   #3,d0
                movea.l ACTIVE_RECORD_POINTER.l,a0
                move.w  $56(a0),d1
                move.w  d0,-$4(a6)
                move.w  d1,-$6(a6)
                tst.w   d1
                bpl.b   $C14960
