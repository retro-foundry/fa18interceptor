; Byte-exact static-only bounded sweep $C33FB4-$C34065.
                org     $C33FB4
POSTFLIGHT_HORIZONTAL_OFFSET    equ $C45988
POSTFLIGHT_RESULT_WORD          equ $C4598C
POSTFLIGHT_SUBMIT_A             equ $C2F60A
POSTFLIGHT_SUBMIT_B             equ $C2F5F4
POSTFLIGHT_RETURN                equ $C33FB2
run_postflight_bounded_sweep:
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                ble.s   POSTFLIGHT_RETURN
                cmpi.w  #$13F,d0
                bge.s   POSTFLIGHT_RETURN
                link.w  a6,#-8
                move.w  d0,-$4(a6)
                move.w  d1,-$6(a6)
                add.w   d1,d0
                move.w  POSTFLIGHT_RESULT_WORD.l,d1
                move.w  d1,-(a7)
                jsr     POSTFLIGHT_SUBMIT_A.l
                move.w  (a7)+,d1
                move.w  #0,-$2(a6)
postflight_sweep_down:
                move.w  -$4(a6),d0
                subq.w  #3,d1
                cmpi.w  #$47,d1
                ble.s   postflight_sweep_up_start
                move.w  d1,-(a7)
                cmpi.w  #4,-$2(a6)
                beq.s   postflight_sweep_down_submit_a
                cmpi.w  #9,-$2(a6)
                beq.s   postflight_sweep_down_submit_a
                jsr     POSTFLIGHT_SUBMIT_B.l
                bra.s   postflight_sweep_down_continue
postflight_sweep_down_submit_a:
                add.w   -$6(a6),d0
                jsr     POSTFLIGHT_SUBMIT_A.l
postflight_sweep_down_continue:
                move.w  (a7)+,d1
                addq.w  #1,-$2(a6)
                bra.s   postflight_sweep_down
postflight_sweep_up_start:
                move.w  POSTFLIGHT_RESULT_WORD.l,d1
                move.w  #0,-$2(a6)
postflight_sweep_up:
                move.w  -$4(a6),d0
                addq.w  #3,d1
                cmpi.w  #$70,d1
                bge.s   postflight_sweep_done
                move.w  d1,-(a7)
                cmpi.w  #4,-$2(a6)
                beq.s   postflight_sweep_up_submit_a
                cmpi.w  #9,-$2(a6)
                beq.s   postflight_sweep_up_submit_a
                jsr     POSTFLIGHT_SUBMIT_B.l
                bra.s   postflight_sweep_up_continue
postflight_sweep_up_submit_a:
                add.w   -$6(a6),d0
                jsr     POSTFLIGHT_SUBMIT_A.l
postflight_sweep_up_continue:
                move.w  (a7)+,d1
                addq.w  #1,-$2(a6)
                bra.s   postflight_sweep_up
postflight_sweep_done:
                unlk    a6
                rts
