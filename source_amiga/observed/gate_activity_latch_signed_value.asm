; Byte-exact signed activity-latch guard $C2540A-$C2540D.
; Negative values bypass the latch decrement/publication path.

                org     $C2540A

gate_activity_latch_signed_value:
                tst.b   d1
                blt.b   $C25416
