; Byte-exact input-phase routine $C0F3C4-$C0F4A5.
; Bounded under both no-input and recorded-key queue-drain packets.

                org     $C0F3C4

INPUT_STATE_WORD               equ $C4577E
INPUT_STATE_MIRROR             equ $C45780
INPUT_MODE_BYTE                equ $C4584B
PENDING_COMMAND_WORDS          equ $C4599A
PENDING_COMMAND_WORDS_2        equ $C4599C
PUBLISHED_COMMAND_WORDS        equ $C45996
PUBLISHED_COMMAND_WORDS_2      equ $C45998

PREPARE_INPUT_STATE            equ $C16EAE
UPDATE_INPUT_STATE             equ $C1715C
HANDLE_INPUT_STATE_CHANGE      equ $C13D34
WAIT_INPUT_EVENT               equ $C1AC28
KEYBOARD_POLL                  equ $C16C56
KEYBOARD_DISPATCH              equ $C1AD74

NO_KEY                         equ $FF
MODE_ONE                       equ 1
MODE_TWO                       equ 2
MODE_THREE                     equ 3

process_pending_key_events:
                link.w  a6,#-6
                jsr     PREPARE_INPUT_STATE.l
                jsr     UPDATE_INPUT_STATE.l
                move.w  INPUT_STATE_WORD.l,d1
                move.w  d0,-6(a6)
                cmp.w   d0,d1
                beq.s   .read_input_mode
                move.w  d0,INPUT_STATE_WORD.l
                move.w  d0,INPUT_STATE_MIRROR.l
                jsr     HANDLE_INPUT_STATE_CHANGE.l
.read_input_mode:
                move.b  INPUT_MODE_BYTE.l,d0
                move.b  d0,-2(a6)
                cmpi.b  #MODE_ONE,d0
                beq.s   .wait_mode_one
                subq.b  #MODE_TWO,d0
                bne.s   .check_mode_three
.wait_mode_one:
                tst.w   PUBLISHED_COMMAND_WORDS.l
                beq.s   .read_key
.wait_mode_one_loop:
                jsr     WAIT_INPUT_EVENT.l
                bra.s   .wait_mode_one
.check_mode_three:
                cmpi.b  #MODE_THREE,-2(a6)
                bne.s   .read_key
.wait_mode_three:
                move.w  PUBLISHED_COMMAND_WORDS.l,d0
                move.w  PUBLISHED_COMMAND_WORDS_2.l,d1
                or.w    d1,d0
                tst.w   d0
                beq.s   .read_key
                jsr     WAIT_INPUT_EVENT.l
                bra.s   .wait_mode_three
.read_key:
                jsr     KEYBOARD_POLL.l
                move.b  d0,-1(a6)
                cmpi.b  #NO_KEY,d0
                beq.s   .publish_pending_commands
                move.b  d0,-3(a6)
                andi.l  #$FF,d0
                move.l  d0,-(a7)
                jsr     KEYBOARD_DISPATCH.l
                addq.l  #4,a7
                bra.s   .read_key
.publish_pending_commands:
                tst.b   -2(a6)
                bne.s   .publish_mode_one
                move.w  PENDING_COMMAND_WORDS.l,PUBLISHED_COMMAND_WORDS.l
                move.w  PENDING_COMMAND_WORDS_2.l,PUBLISHED_COMMAND_WORDS_2.l
                moveq   #0,d0
                move.w  d0,PENDING_COMMAND_WORDS.l
                move.w  d0,PENDING_COMMAND_WORDS_2.l
                bra.s   .return
.publish_mode_one:
                cmpi.b  #MODE_ONE,-2(a6)
                bne.s   .return
                move.w  PENDING_COMMAND_WORDS_2.l,PUBLISHED_COMMAND_WORDS_2.l
                clr.w   PENDING_COMMAND_WORDS_2.l
.return:
                unlk    a6
                rts
