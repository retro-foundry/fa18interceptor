; Byte-exact static-only bounded submission setup $C3377A-$C337DB.
                org     $C3377A
POSTFLIGHT_HORIZONTAL_OFFSET    equ $C45988
POSTFLIGHT_VERTICAL_OFFSET      equ $C458D8
POSTFLIGHT_SETUP_HELPER_A       equ $C33FB4
POSTFLIGHT_SETUP_HELPER_B       equ $C33B06
POSTFLIGHT_SUBMIT_A             equ $C2F60A
POSTFLIGHT_SUBMIT_B             equ $C2F5C0
POSTFLIGHT_SUBMIT_C             equ $C2F5D4
run_postflight_bounded_submission_setup:
                move.w  #$69,d0
                move.w  #0,d1
                bsr.w   POSTFLIGHT_SETUP_HELPER_A
                move.w  #$6B,d0
                bsr.w   POSTFLIGHT_SETUP_HELPER_B
                move.w  #$6E,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                cmpi.w  #5,d0
                ble.s   postflight_bounded_submission_done
                cmpi.w  #$13B,d0
                bge.s   postflight_bounded_submission_done
                move.w  #$56,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                jsr     POSTFLIGHT_SUBMIT_A.l
                move.w  #$6C,d0
                move.w  #$57,d1
                jsr     POSTFLIGHT_SUBMIT_B.l
                addq.w  #1,d1
                jsr     POSTFLIGHT_SUBMIT_C.l
                move.w  #$6E,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                addq.w  #1,d1
                jsr     POSTFLIGHT_SUBMIT_A.l
postflight_bounded_submission_done:
