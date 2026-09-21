; Byte-exact observed delta branch $C2DDCC-$C2DDD1.

                org     $C2DDCC

CONTINUE_RECORD_ADJUSTMENT_DELTA equ $C2DDE8

branch_record_adjustment_delta:
                sub.w   d4,d1
                beq.b   CONTINUE_RECORD_ADJUSTMENT_DELTA
