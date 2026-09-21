; Byte-exact static-only four-step sweep prefix $C34066-$C340DB.
                org     $C34066
POSTFLIGHT_RESULT_WORD          equ $C4598C
POSTFLIGHT_HORIZONTAL_OFFSET    equ $C45988
POSTFLIGHT_SUBMIT_A             equ $C2F5C0
POSTFLIGHT_SUBMIT_B             equ $C2F5D4
POSTFLIGHT_SUBMIT_C             equ $C2F5F4
POSTFLIGHT_SECOND_SWEEP_START   equ $C340DC
run_postflight_four_step_sweep_prefix:
                link.w  a6,#-8
                move.w  d1,-$4(a6)
                move.w  POSTFLIGHT_RESULT_WORD.l,d0
                jsr     POSTFLIGHT_SUBMIT_A.l
                blt.s   postflight_four_step_setup_done
                subq.w  #1,d1
                jsr     POSTFLIGHT_SUBMIT_B.l
postflight_four_step_setup_done:
                move.w  #0,-$2(a6)
postflight_four_step_first_sweep:
                move.w  -$4(a6),d1
                addi.w  #$A,d0
                blt.s   POSTFLIGHT_SECOND_SWEEP_START
                cmpi.w  #$13F,d0
                bgt.s   POSTFLIGHT_SECOND_SWEEP_START
                move.w  #$B9,d2
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d2
                cmp.w   d2,d0
                bge.s   POSTFLIGHT_SECOND_SWEEP_START
                move.w  d0,-(a7)
                cmpi.w  #1,-$2(a6)
                beq.s   postflight_four_step_first_special
                cmpi.w  #3,-$2(a6)
                beq.s   postflight_four_step_first_special
                jsr     POSTFLIGHT_SUBMIT_C.l
                bra.s   postflight_four_step_first_continue
postflight_four_step_first_special:
                jsr     POSTFLIGHT_SUBMIT_B.l
                move.w  -$4(a6),d1
                subq.w  #1,d1
                jsr     POSTFLIGHT_SUBMIT_C.l
postflight_four_step_first_continue:
                move.w  (a7)+,d0
                addq.w  #1,-$2(a6)
                bra.s   postflight_four_step_first_sweep
