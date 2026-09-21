; Byte-exact observed command-event lookup prefix $C32E5C-$C32E7B.
; The nonzero event continuation remains raw.

                org     $C32E5C

MESSAGE_EVENT_SIGNED_GUARD      equ     $C457F5
MESSAGE_EVENT_BYTES             equ     $C457E1
MESSAGE_EVENT_BYTE_INDEX        equ     $C457F8
RETURN_NO_COMMAND_EVENT         equ     $C32CEC

lookup_message_command_event:
                tst.b   MESSAGE_EVENT_SIGNED_GUARD.l
                blt.w   $C32EAE
                lea.l   MESSAGE_EVENT_BYTES.l,a0
                move.b  MESSAGE_EVENT_BYTE_INDEX.l,d5
                ext.w   d5
                move.b  (a0,d5.w),d4
                beq.w   RETURN_NO_COMMAND_EVENT
