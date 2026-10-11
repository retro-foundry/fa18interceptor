; Byte-exact shared fall-through $C32D1E-$C32D23.
; Control-word ownership is unresolved; execution continues at $C32D24.

                org     $C32D1E

MESSAGE_COMMAND_CONTROL_BYTE   equ $C457F6
SELECT_MESSAGE_RECORD          equ $C32D24

clear_message_control_before_record_selection:
                clr.b   MESSAGE_COMMAND_CONTROL_BYTE.l
                ; Fall through directly into SELECT_MESSAGE_RECORD.
