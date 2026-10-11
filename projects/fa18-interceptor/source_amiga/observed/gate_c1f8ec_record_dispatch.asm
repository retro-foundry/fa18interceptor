; Byte-exact record-dispatch threshold gate $C1F8EC-$C1F90F.

                org     $C1F8EC

STREAM_STAGE_SHIFT               equ $C45AB8

gate_c1f8ec_record_dispatch:
                bra.w   $C1F7FA
                move.w  (a2)+,d0
                blt.b   $C1F8EC
                move.w  STREAM_STAGE_SHIFT.l,d1
                asr.w   d1,d0
                cmp.w   -40(a6),d0
                blt.b   $C1F8EC
                bra.w   $C1F966
                clr.w   -106(a6)
                tst.w   -106(a6)
                bne.b   $C1F8EC
