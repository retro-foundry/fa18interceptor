; Byte-exact observed record-stage gate $C25A68-$C25A73.

                org     $C25A68

RECORD_STAGE_ENABLE             equ $C45790

record_stage_return:
                rts

check_record_stage_enable:
                tst.b   RECORD_STAGE_ENABLE.l
                beq.s   record_stage_return
