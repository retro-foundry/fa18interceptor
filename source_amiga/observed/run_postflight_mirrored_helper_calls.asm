; Byte-exact static-only mirrored helper calls $C334FA-$C335A1.
                org     $C334FA
POSTFLIGHT_PACKED_VALUE       equ $C45B22
POSTFLIGHT_BCD_LEFT           equ $C45B26
POSTFLIGHT_BCD_RIGHT          equ $C45B2A
POSTFLIGHT_RENDERER_SETUP     equ $C2F5C0
POSTFLIGHT_HELPER             equ $C33F8A
run_postflight_mirrored_helper_calls:
                move.l  POSTFLIGHT_PACKED_VALUE.l,-(a7)
                move.l  d5,-(a7)
                move.w  d1,-(a7)
postflight_negative_pass:
                subi.l  #$258,d5
                cmpi.l  #$AF0,d5
                blt.s   postflight_restore_primary_state
                subi.w  #$F,d1
                move.w  d1,-(a7)
                move.l  d5,-(a7)
                move.w  #$DF,d0
                jsr     POSTFLIGHT_RENDERER_SETUP.l
                move.l  (a7)+,d5
                move.l  d5,-(a7)
                lea.l   POSTFLIGHT_BCD_LEFT.l,a1
                lea.l   POSTFLIGHT_BCD_RIGHT.l,a2
                addi.w  #0,d0
                abcd.b  -(a2),-(a1)
                abcd.b  -(a2),-(a1)
                abcd.b  -(a2),-(a1)
                bsr.w   POSTFLIGHT_HELPER
                move.l  (a7)+,d5
                move.w  (a7)+,d1
                bra.s   postflight_negative_pass
postflight_restore_primary_state:
                move.w  (a7)+,d1
                move.l  (a7)+,d5
                move.l  (a7)+,POSTFLIGHT_PACKED_VALUE.l
postflight_positive_pass:
                tst.l   POSTFLIGHT_PACKED_VALUE.l
                beq.w   postflight_mirrored_calls_done
                addi.l  #$258,d5
                cmpi.l  #$1130,d5
                bgt.w   postflight_mirrored_calls_done
                addi.w  #$F,d1
                move.w  d1,-(a7)
                move.l  d5,-(a7)
                move.w  #$DF,d0
                jsr     POSTFLIGHT_RENDERER_SETUP.l
                move.l  (a7)+,d5
                move.l  d5,-(a7)
                lea.l   POSTFLIGHT_BCD_LEFT.l,a1
                lea.l   POSTFLIGHT_BCD_RIGHT.l,a2
                addi.w  #0,d0
                sbcd.b  -(a2),-(a1)
                sbcd.b  -(a2),-(a1)
                sbcd.b  -(a2),-(a1)
                bsr.w   POSTFLIGHT_HELPER
                move.l  (a7)+,d5
                move.w  (a7)+,d1
                bra.s   postflight_positive_pass
postflight_mirrored_calls_done:
