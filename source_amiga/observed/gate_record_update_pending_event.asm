; Byte-exact record-update pending-event gate $C23CA6-$C23CAF.

                org     $C23CA6

PENDING_RECORD_EVENT             equ $C457AE

gate_record_update_pending_event:
                tst.b   PENDING_RECORD_EVENT.l
                bne.w   $C23D7A
