; Byte-exact low-mode selector $C1B5B8-$C1B5DB.
; Raw $0B/$0C dispatcher entries reach the first two selectors.

                org     $C1B5B8

THROTTLE_MODE_FLAGS            equ $C461E9
THROTTLE_MODE_MASK             equ $FC
RESET_THROTTLE_INPUT_STATE      equ $C1B602
SHARED_COMMAND_FALLBACK         equ $C1C23C

select_throttle_mode_one:
                moveq   #1,d2
                bra.s   apply_throttle_mode
select_throttle_mode_two:
                moveq   #2,d2
                bra.s   apply_throttle_mode
select_throttle_mode_zero:
                moveq   #0,d2
apply_throttle_mode:
                move.b  THROTTLE_MODE_FLAGS.l,d1
                andi.b  #THROTTLE_MODE_MASK,d1
                or.b    d2,d1
                move.b  d1,THROTTLE_MODE_FLAGS.l
                bsr.w   RESET_THROTTLE_INPUT_STATE
                bra.w   SHARED_COMMAND_FALLBACK
