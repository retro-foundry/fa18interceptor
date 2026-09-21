; Byte-exact static prefix of the post-input tick $C0F5F8-$C0F7D1.
; The bounded training trace takes the first guard directly to the separately
; reconstructed tail at $C0F7D2. Other branches are retained structurally.

                org     $C0F5F8

POST_TICK_ENTRY_GUARD           equ $C45898
POST_TICK_ENABLE                equ $C458A6
POST_TICK_PHASE                 equ $C4582A
POST_TICK_PHASE_FLAG            equ $C4582B
POST_TICK_SIGNED_GUARD          equ $C4582C
POST_TICK_SECONDARY_GUARD       equ $C45790
POST_TICK_LATER_GUARD           equ $C45798
POST_TICK_MODE_FLAG             equ $C458AC
POST_TICK_MODE_BYTE             equ $C458A1
POST_TICK_MODE_LATCH            equ $C458AD
POST_TICK_EVENT_FLAG            equ $C457AE
POST_TICK_RESULT_FLAG           equ $C457C5
POST_TICK_AUXILIARY_BYTE        equ $C45795
POST_TICK_COUNTER_SOURCE        equ $C45AF2
POST_TICK_PRIMARY_OFFSET        equ $C45914
POST_TICK_SECONDARY_OFFSET      equ $C45904
POST_TICK_TERTIARY_OFFSET       equ $C45908
POST_TICK_QUATERNARY_OFFSET     equ $C45910
POST_TICK_ADDITIONAL_OFFSET     equ $C4590C
POST_TICK_RESULT_CODE           equ $C4599E
POST_TICK_COUNTDOWN             equ $C45AD6
POST_TICK_CONFIGURED_COUNTDOWN  equ $C458C0
POST_TICK_RESULT_TARGET         equ $C1AB74
POST_INPUT_CALLBACK_POINTER     equ $C1820C

POST_TICK_OFFSET_LIMIT          equ $4650
POST_TICK_RESULT_CODE_VALUE     equ $003F
POST_TICK_PHASE_DISABLED        equ $FF
POST_TICK_LATER_GUARD_VALUE     equ $F0
POST_TICK_SHORT_COUNTDOWN       equ 3
POST_TICK_LONG_COUNTDOWN        equ 6

CALL_C06C02                     equ $C06C02
POST_TICK_CALLBACK_ZERO         equ $C0F920
POST_TICK_CALLBACK_ONE          equ $C0F946
POST_TICK_CALLBACK_TWO          equ $C1104C
POST_TICK_CALLBACK_THREE        equ $C11078
run_post_input_tick_tail        equ $C0F7D2

run_post_input_tick_prefix:
                link.w  a6,#-4
                move.b  POST_TICK_ENTRY_GUARD.l,d0
                tst.b   d0
                bmi.w   run_post_input_tick_tail
                move.b  POST_TICK_ENABLE.l,d0
                tst.b   d0
                ble.w   run_post_input_tick_tail
                tst.b   POST_TICK_PHASE.l
                beq.w   update_post_tick_phase
                move.b  POST_TICK_SECONDARY_GUARD.l,d0
                tst.b   d0
                bne.w   update_post_tick_phase
                move.b  POST_TICK_SIGNED_GUARD.l,d0
                tst.b   d0
                bpl.w   update_post_tick_phase
                tst.l   POST_TICK_PRIMARY_OFFSET.l
                beq.w   update_post_tick_phase
                move.l  POST_TICK_COUNTER_SOURCE.l,d0
                sub.l   POST_TICK_PRIMARY_OFFSET.l,d0
                sub.l   POST_TICK_TERTIARY_OFFSET.l,d0
                move.l  d0,-4(a6)
                tst.l   POST_TICK_SECONDARY_OFFSET.l
                beq.b   subtract_quaternary_offset
                move.l  POST_TICK_COUNTER_SOURCE.l,d1
                sub.l   POST_TICK_SECONDARY_OFFSET.l,d1
                sub.l   d1,-4(a6)

subtract_quaternary_offset:
                move.l  -4(a6),d0
                sub.l   POST_TICK_QUATERNARY_OFFSET.l,d0
                move.l  d0,-4(a6)
                tst.l   POST_TICK_ADDITIONAL_OFFSET.l
                beq.b   check_post_tick_offset
                move.l  POST_TICK_COUNTER_SOURCE.l,d1
                sub.l   POST_TICK_ADDITIONAL_OFFSET.l,d1
                sub.l   d1,-4(a6)

check_post_tick_offset:
                tst.l   -4(a6)
                bmi.b   handle_invalid_post_tick_offset
                cmpi.l  #POST_TICK_OFFSET_LIMIT,-4(a6)
                bge.b   handle_invalid_post_tick_offset
                move.l  -4(a6),d0
                movea.l POST_TICK_RESULT_TARGET.l,a0
                add.l   d0,8(a0)
                move.b  #1,POST_TICK_RESULT_FLAG.l
                bra.b   clear_post_tick_primary_offset

handle_invalid_post_tick_offset:
                move.w  #POST_TICK_RESULT_CODE_VALUE,POST_TICK_RESULT_CODE.l
                jsr     CALL_C06C02.l

clear_post_tick_primary_offset:
                clr.l   POST_TICK_PRIMARY_OFFSET.l

update_post_tick_phase:
                move.b  POST_TICK_LATER_GUARD.l,d0
                tst.b   d0
                bmi.w   check_post_tick_phase_three
                move.b  POST_TICK_PHASE.l,d0
                cmpi.b  #POST_TICK_PHASE_DISABLED,d0
                bne.b   check_post_tick_phase_one
                moveq   #0,d0
                move.b  d0,POST_TICK_PHASE.l
                move.b  d0,POST_TICK_PHASE_FLAG.l
                lea.l   POST_TICK_CALLBACK_ZERO(pc),a0
                move.l  a0,POST_INPUT_CALLBACK_POINTER.l
                bra.w   run_post_input_tick_tail

check_post_tick_phase_one:
                move.b  POST_TICK_PHASE.l,d0
                subq.b  #1,d0
                bne.b   check_post_tick_phase_two
                clr.b   POST_TICK_PHASE.l
                tst.b   POST_TICK_MODE_FLAG.l
                beq.b   request_post_tick_event
                move.b  #1,POST_TICK_PHASE_FLAG.l

request_post_tick_event:
                move.b  #1,POST_TICK_EVENT_FLAG.l
                move.w  #POST_TICK_SHORT_COUNTDOWN,POST_TICK_COUNTDOWN.l
                moveq   #0,d0
                move.b  d0,POST_TICK_AUXILIARY_BYTE.l
                move.b  d0,POST_TICK_MODE_BYTE.l
                lea.l   POST_TICK_CALLBACK_ONE(pc),a0
                move.l  a0,POST_INPUT_CALLBACK_POINTER.l
                bra.w   run_post_input_tick_tail

check_post_tick_phase_two:
                move.b  POST_TICK_PHASE.l,d0
                subq.b  #2,d0
                bne.b   run_post_input_tick_tail
                moveq   #0,d0
                move.b  d0,POST_TICK_PHASE_FLAG.l
                move.b  POST_TICK_SIGNED_GUARD.l,d0
                tst.b   d0
                bpl.b   run_post_input_tick_tail
                move.b  #POST_TICK_LATER_GUARD_VALUE,POST_TICK_LATER_GUARD.l
                clr.b   POST_TICK_PHASE.l
                moveq   #1,d0
                move.b  d0,POST_TICK_MODE_LATCH.l
                move.b  d0,POST_TICK_EVENT_FLAG.l
                move.w  #POST_TICK_LONG_COUNTDOWN,POST_TICK_COUNTDOWN.l
                lea.l   POST_TICK_CALLBACK_TWO(pc),a0
                move.l  a0,POST_INPUT_CALLBACK_POINTER.l
                bra.b   run_post_input_tick_tail

check_post_tick_phase_three:
                move.b  POST_TICK_PHASE.l,d0
                subq.b  #3,d0
                bne.b   run_post_input_tick_tail
                moveq   #0,d0
                move.b  d0,POST_TICK_PHASE_FLAG.l
                move.b  POST_TICK_SIGNED_GUARD.l,d0
                tst.b   d0
                bpl.b   run_post_input_tick_tail
                clr.b   POST_TICK_PHASE.l
                move.w  POST_TICK_CONFIGURED_COUNTDOWN.l,POST_TICK_COUNTDOWN.l
                lea.l   POST_TICK_CALLBACK_THREE(pc),a0
                move.l  a0,POST_INPUT_CALLBACK_POINTER.l
