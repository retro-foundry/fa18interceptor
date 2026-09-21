; Byte-exact static-only bounded helper entries $C33AD6-$C33B35.
                org     $C33AD6
POSTFLIGHT_HORIZONTAL_OFFSET    equ $C45988
POSTFLIGHT_VERTICAL_OFFSET      equ $C458D8
POSTFLIGHT_LINE_HELPER          equ $C2FA7E
run_postflight_bounded_helper_a:
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                cmpi.w  #$13B,d0
                bge.s   postflight_bounded_helper_a_done
                move.w  #$5A,d1
                move.w  d0,d2
                addq.w  #4,d2
                cmpi.w  #4,d2
                ble.s   postflight_bounded_helper_a_done
                move.w  d1,d3
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d3
                jsr     POSTFLIGHT_LINE_HELPER.l
postflight_bounded_helper_a_done:
                rts
run_postflight_bounded_helper_b:
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                cmpi.w  #$13B,d0
                bge.s   postflight_bounded_helper_b_done
                move.w  #$5A,d1
                move.w  d0,d2
                addq.w  #4,d2
                cmpi.w  #4,d2
                ble.s   postflight_bounded_helper_b_done
                move.w  d1,d3
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d3
                jsr     POSTFLIGHT_LINE_HELPER.l
postflight_bounded_helper_b_done:
                rts
