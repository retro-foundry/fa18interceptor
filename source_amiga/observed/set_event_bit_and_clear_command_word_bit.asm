; Byte-exact sibling event-code effect $C08394-$C083A5.
; Field and gameplay meanings remain unassigned.
                org     $C08394

EVENT_FLAG_BYTE                 equ $C4599A
COMMAND_WORD                    equ $C458C6
EVENT_FLAG_BIT                  equ 3
COMMAND_WORD_CLEAR_BIT_3_MASK   equ $FFF7

set_event_bit_and_clear_command_word_bit:
                bset.b  #EVENT_FLAG_BIT,EVENT_FLAG_BYTE.l
                andi.w  #COMMAND_WORD_CLEAR_BIT_3_MASK,COMMAND_WORD.l
                rts
