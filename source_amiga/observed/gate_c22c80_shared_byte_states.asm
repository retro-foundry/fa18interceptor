; Byte-exact observed C22C80 continuation $C22D2A-$C22D39.
; Two shared byte-state tests select the adjacent record-update routes; their
; higher-level field roles remain unassigned.

                org     $C22D2A

RECORD_UPDATE_STATE_A          equ     $C457AE
RECORD_UPDATE_STATE_B          equ     $C4584F

gate_c22c80_shared_byte_states:
                tst.b   RECORD_UPDATE_STATE_A.l
                bne.b   $C22D4E
                tst.b   RECORD_UPDATE_STATE_B.l
                ble.b   $C22D40
