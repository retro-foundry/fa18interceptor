; Byte-exact base-record mode-six completion $C242D4-$C242DF.

                org     $C242D4

BASE_RECORD_MODE_STATE            equ $C457B7

finish_base_record_update_mode:
                clr.b   BASE_RECORD_MODE_STATE.l
                moveq   #1,d0
                rts
