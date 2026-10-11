; Byte-exact runtime-observed postflight renderer sequence $C332FE-$C3336F.
; Run024 frame-23000 continuation executes the complete helper body.

                org     $C332FE

POSTFLIGHT_RETURN                equ $C332FC
POSTFLIGHT_HORIZONTAL_OFFSET     equ $C45988
POSTFLIGHT_VERTICAL_OFFSET       equ $C458D8
POSTFLIGHT_RENDERER_SELECTOR     equ $C45954
SUBMIT_ADJACENT_RENDERER_VALUES  equ $C2F60A

run_postflight_five_renderer_calls:
                move.w  #$A0,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                cmpi.w  #1,d0
                ble.s   POSTFLIGHT_RETURN
                cmpi.w  #$13D,d0
                bge.s   POSTFLIGHT_RETURN
                move.w  #$81,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                move.w  #8,POSTFLIGHT_RENDERER_SELECTOR.l
                movem.w d0-d1,-(a7)
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                movem.w (a7)+,d0-d1
                addq.w  #1,d1
                movem.w d0-d1,-(a7)
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                movem.w (a7)+,d0-d1
                addq.w  #1,d1
                movem.w d0-d1,-(a7)
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                movem.w (a7)+,d0-d1
                subq.w  #1,d0
                addq.w  #1,d1
                movem.w d0-d1,-(a7)
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                movem.w (a7)+,d0-d1
                addq.w  #2,d0
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                rts
