; Byte-exact observed non-$30 initialization $C2D496-$C2D4AD.

                org     $C2D496

ACTIVE_UPDATE_SELECTOR          equ     $C459B4
RECORD_PATH_GATE                equ     $C457AE
CONTINUE_DEFAULT_RECORD_PATH    equ     $C2D5FC

initialize_nonclass30_record_path:
                moveq   #0,d0
                move.w  d0,d2
                move.w  d0,d4
                tst.w   ACTIVE_UPDATE_SELECTOR.l
                bne.b   $C2D4B2
                tst.b   RECORD_PATH_GATE.l
                beq.w   CONTINUE_DEFAULT_RECORD_PATH
