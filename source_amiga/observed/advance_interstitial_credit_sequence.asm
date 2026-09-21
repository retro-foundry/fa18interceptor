; Byte-exact credit-sequence advance callback at $C1072E-$C10759.

                org     $C1072E

INTERSTITIAL_COUNTER            equ $C457E0
INTERSTITIAL_SELECTOR            equ $C4574A
INTERSTITIAL_ACTIVE_FLAG         equ $C457C3
INTERSTITIAL_CALLBACK_SLOT       equ $C1820C

advance_interstitial_credit_sequence:
                move.b  INTERSTITIAL_COUNTER.l,d0
                subq.b  #1,d0
                bne.b   finish_interstitial_credit_sequence
                move.w  #4,INTERSTITIAL_SELECTOR.l
                clr.b   INTERSTITIAL_ACTIVE_FLAG.l
                move.b  #3,INTERSTITIAL_COUNTER.l
                lea.l   run_current_post_input_callback(pc),a0
                move.l  a0,INTERSTITIAL_CALLBACK_SLOT.l

finish_interstitial_credit_sequence:
                rts

run_current_post_input_callback equ $C1075A
