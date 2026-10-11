; Byte-exact record-update state clear path $C2450E-$C2451B.

                org     $C2450E

RECORD_UPDATE_STATUS             equ $C45846
RECORD_UPDATE_WORD               equ $C459C0

clear_record_update_state:
                clr.b   RECORD_UPDATE_STATUS.l
                move.w  RECORD_UPDATE_WORD.l,d0
                ble.b   $C24566
