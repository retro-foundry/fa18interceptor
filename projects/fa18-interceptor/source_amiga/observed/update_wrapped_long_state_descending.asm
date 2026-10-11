; Byte-exact observed wrapped-long-state continuation $C25012-$C25021.
; It applies the negative $4000 step, accepts nonnegative results into the
; shared long state, and otherwise branches to its zero-value fallback.

                org     $C25012

WRAPPED_LONG_STATE             equ     $C4FF26

update_wrapped_long_state_descending:
                subi.l  #$4000,d1
                blt.b   $C25022
                move.l  d1,WRAPPED_LONG_STATE.l
                rts
