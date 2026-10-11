; Byte-exact static-only postflight renderer configuration $C3112A-$C31223.

                org     $C3112A

POSTFLIGHT_RETURN                equ $C31128
POSTFLIGHT_MASK_LONG             equ $C456E6
POSTFLIGHT_FLAGS                 equ $C4586E
POSTFLIGHT_RENDERER_SELECTOR     equ $C45954
POSTFLIGHT_RECORD_BASE           equ $C46184
POSTFLIGHT_RECORD_OFFSET         equ $C458DE
POSTFLIGHT_AUX_FLAGS             equ $C458DB
SUBMIT_OFFSET_RENDERER_VALUES    equ $C2F63A
SUBMIT_ALTERNATE_RENDERER_VALUES equ $C2F64E

configure_postflight_renderer_values:
                move.l  #$000FFFFF,POSTFLIGHT_MASK_LONG.l
                btst    #2,POSTFLIGHT_FLAGS.l
                bne.s   postflight_renderer_first_set
                move.w  #3,d2
                bra.s   postflight_renderer_first_submit
postflight_renderer_first_set:
                move.w  #1,d2
postflight_renderer_first_submit:
                move.w  d2,POSTFLIGHT_RENDERER_SELECTOR.l
                move.w  #$125,d0
                move.w  #$9C,d1
                jsr     SUBMIT_OFFSET_RENDERER_VALUES.l
                blt.s   POSTFLIGHT_RETURN
                addq.w  #2,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                btst    #1,POSTFLIGHT_FLAGS.l
                bne.s   postflight_renderer_second_set
                move.w  #3,d2
                bra.s   postflight_renderer_second_submit
postflight_renderer_second_set:
                move.w  #4,d2
postflight_renderer_second_submit:
                move.w  d2,POSTFLIGHT_RENDERER_SELECTOR.l
                addq.w  #3,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                addq.w  #2,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                btst    #4,POSTFLIGHT_FLAGS.l
                bne.s   postflight_renderer_third_set
                move.w  #3,d2
                bra.s   postflight_renderer_third_submit
postflight_renderer_third_set:
                move.w  #2,d2
postflight_renderer_third_submit:
                move.w  d2,POSTFLIGHT_RENDERER_SELECTOR.l
                subq.w  #2,d0
                addq.w  #3,d1
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                addq.w  #2,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                btst    #5,POSTFLIGHT_FLAGS.l
                bne.s   postflight_renderer_fourth_set
                move.w  #3,d2
                bra.s   postflight_renderer_fourth_submit
postflight_renderer_fourth_set:
                move.w  #2,d2
postflight_renderer_fourth_submit:
                move.w  d2,POSTFLIGHT_RENDERER_SELECTOR.l
                subq.w  #7,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                addq.w  #2,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                lea     POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a0
                move.w  #3,d2
                btst    #2,$20(a0)
                beq.s   postflight_renderer_final_submit
                btst    #1,POSTFLIGHT_AUX_FLAGS.l
                beq.s   postflight_renderer_final_submit
                move.w  #1,d2
postflight_renderer_final_submit:
                move.w  d2,POSTFLIGHT_RENDERER_SELECTOR.l
                addq.w  #8,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                addq.w  #2,d0
                jsr     SUBMIT_ALTERNATE_RENDERER_VALUES.l
                rts
