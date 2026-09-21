; Byte-exact command-dispatch continuation $C1B21C-$C1B263.

                org     $C1B21C

SHARED_COMMAND_FALLBACK          equ $C1C23C
SPACE_COMMAND_HELPER             equ $C0833E
ALTERNATE_COMMAND_HELPER         equ $C08394
MODE_CYCLE_WORD                  equ $C459C4
MODE_CYCLE_MAXIMUM               equ 3
MODE_CYCLE_INITIAL               equ 1
MODE_CYCLE_FLAG                  equ 15
MODE_CYCLE_STATE                 equ $C45886
MODE_CYCLE_STATE_VALUE           equ 5
MODE_CYCLE_SECONDARY_STATE       equ $C4583C
MODE_CYCLE_SECONDARY_VALUE       equ 2

dispatch_space_and_mode_commands:
                tst.b   d6
                bne.w   SHARED_COMMAND_FALLBACK
                jsr     SPACE_COMMAND_HELPER
                bra.w   SHARED_COMMAND_FALLBACK

dispatch_alternate_command:
                jsr     ALTERNATE_COMMAND_HELPER
                bra.w   SHARED_COMMAND_FALLBACK

cycle_mode_command:
                move.w  MODE_CYCLE_WORD.l,d0
                addq.w  #1,d0
                cmpi.w  #MODE_CYCLE_MAXIMUM,d0
                ble.s   .store_mode
                moveq   #MODE_CYCLE_INITIAL,d0
.store_mode:
                bset    #MODE_CYCLE_FLAG,d0
                move.w  d0,MODE_CYCLE_WORD.l
                move.b  #MODE_CYCLE_STATE_VALUE,MODE_CYCLE_STATE.l
                move.b  #MODE_CYCLE_SECONDARY_VALUE,MODE_CYCLE_SECONDARY_STATE.l
                bra.w   SHARED_COMMAND_FALLBACK
