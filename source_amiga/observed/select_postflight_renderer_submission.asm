; Byte-exact static-only postflight renderer selection $C31612-$C316BF.

                org     $C31612

POSTFLIGHT_HORIZONTAL_OFFSET     equ $C45988
POSTFLIGHT_VERTICAL_OFFSET       equ $C458D8
POSTFLIGHT_RECORD_FLAGS          equ $C45883
POSTFLIGHT_RECORD_BASE           equ $C46184
POSTFLIGHT_SELECTED_INDEX        equ $C459C0
POSTFLIGHT_COMPARISON_LONG       equ $C45A78
POSTFLIGHT_RENDERER_SELECTOR     equ $C45954
POSTFLIGHT_SUBMISSION_SKIP       equ $C3170E

select_postflight_renderer_submission:
                addi.w  #$9E,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                blt.w   POSTFLIGHT_SUBMISSION_SKIP
                cmpi.w  #$140,d0
                bge.w   POSTFLIGHT_SUBMISSION_SKIP
                addi.w  #$A7,d1
                btst    #4,d3
                beq.w   POSTFLIGHT_SUBMISSION_SKIP
                move.l  POSTFLIGHT_COMPARISON_LONG.l,d5
                move.w  d3,d4
                andi.w  #$FF00,d4
                add.w   d4,d4
                cmp.w   POSTFLIGHT_SELECTED_INDEX.l,d4
                bne.s   postflight_submission_load_record
                btst    #0,POSTFLIGHT_RECORD_FLAGS.l
                beq.w   POSTFLIGHT_SUBMISSION_SKIP
postflight_submission_load_record:
                move.l  a0,-(a7)
                lea     POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  d4,a0
                neg.l   d5
                bclr    #0,d7
                cmp.l   $10(a0),d5
                bgt.s   postflight_submission_compare_done
                bset    #0,d7
postflight_submission_compare_done:
                move.b  $62(a0),d5
                andi.b  #$F0,d5
                cmpi.b  #$20,d5
                beq.s   postflight_submission_type_five
                cmpi.b  #$30,d5
                beq.s   postflight_submission_type_five
                btst    #6,$20(a0)
                beq.s   postflight_submission_type_eight
                btst    #1,$20(a0)
                bne.s   postflight_submission_type_eight
                btst    #3,$1(a0)
                bne.s   postflight_submission_type_four
                ; CMPI.B #0,D5. VASM otherwise substitutes TST.B D5.
                dc.w    $0C05,$0000
                beq.s   postflight_submission_type_two
                moveq   #1,d2
                bra.s   postflight_submission_store
postflight_submission_type_four:
                moveq   #4,d2
                bra.s   postflight_submission_store
postflight_submission_type_two:
                move.w  #2,d2
                bra.s   postflight_submission_store
postflight_submission_type_five:
                move.w  #5,d2
                bra.s   postflight_submission_store
postflight_submission_type_eight:
                moveq   #8,d2
postflight_submission_store:
                move.w  d2,POSTFLIGHT_RENDERER_SELECTOR.l
                movea.l (a7)+,a0
