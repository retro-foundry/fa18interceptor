; Byte-exact static-only submission prefix $C337DC-$C337FB.
                org     $C337DC
POSTFLIGHT_SUBMIT_BASE          equ $C2F5C0
POSTFLIGHT_SUBMIT_FOLLOWUP      equ $C2F5D4
POSTFLIGHT_SUBMIT_FINAL         equ $C2F5F4
run_postflight_submission_prefix:
                move.w  #$9F,d0
                move.w  #$3F,d1
                jsr     POSTFLIGHT_SUBMIT_BASE.l
                blt.s   postflight_submission_prefix_done
                addq.w  #1,d1
                jsr     POSTFLIGHT_SUBMIT_FOLLOWUP.l
                addq.w  #1,d1
                jsr     POSTFLIGHT_SUBMIT_FINAL.l
postflight_submission_prefix_done:
