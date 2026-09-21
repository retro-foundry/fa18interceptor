; Byte-exact shared record-update cleanup $C230A6-$C230AF.

                org     $C230A6

FINAL_RECORD_UPDATE_HELPER      equ $C09E06

return_c230a6_record_update_stage:
                jsr     FINAL_RECORD_UPDATE_HELPER.l
                move.l  (sp)+,d5
                rts
