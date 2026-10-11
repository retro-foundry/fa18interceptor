; Byte-exact static-only transform-helper entries $C33F54-$C33FB1.
                org     $C33F54
POSTFLIGHT_WORK_BASE            equ $C457FA
POSTFLIGHT_TRANSFORM_SUBMIT     equ $C32AA6
run_postflight_transform_helper_a:
                moveq   #1,d4
                move.w  #$10,d0
                move.l  #$858,d5
                moveq   #1,d6
                moveq   #2,d7
                lea.l   POSTFLIGHT_WORK_BASE.l,a2
                lea.l   $3(a2),a0
                bra.s   postflight_transform_submit
run_postflight_transform_helper_b:
                moveq   #0,d4
                lea.l   $C33264(pc),a1
                move.w  #$A,d0
                moveq   #2,d6
                moveq   #2,d7
                lea.l   POSTFLIGHT_WORK_BASE.l,a2
                lea.l   $3(a2),a0
                bra.s   postflight_transform_submit
run_postflight_transform_helper_c:
                moveq   #1,d4
                lea.l   $C33258(pc),a1
                move.w  #$1A,d0
                moveq   #2,d6
                moveq   #2,d7
                lea.l   POSTFLIGHT_WORK_BASE.l,a2
                lea.l   $3(a2),a0
postflight_transform_submit:
                move.w  d7,d2
                movea.l d5,a4
                swap    d0
                move.w  d6,d0
                jsr     POSTFLIGHT_TRANSFORM_SUBMIT.l
                rts
postflight_transform_helper_return:
                rts
