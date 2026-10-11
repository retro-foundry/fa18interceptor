; Byte-exact static-only ten-step prefix $C338AA-$C33919.
                org     $C338AA
POSTFLIGHT_PACKED_VALUE         equ $C45B22
POSTFLIGHT_BCD_LEFT             equ $C45B26
POSTFLIGHT_BCD_RIGHT            equ $C45B2A
POSTFLIGHT_TABLE_BASE           equ $C33A16
POSTFLIGHT_SCALED_HELPER        equ $C33F54
run_postflight_ten_step_prefix:
                move.l  #$A,POSTFLIGHT_BCD_LEFT.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,-(a7)
                move.w  d1,-(a7)
postflight_ten_step_forward:
                addi.w  #$A0,d1
                cmpi.w  #$B0,d1
                bgt.s   postflight_ten_step_restore_state
                lea.l   POSTFLIGHT_BCD_LEFT.l,a1
                lea.l   POSTFLIGHT_BCD_RIGHT.l,a2
                addi.w  #0,d0
                abcd.b  -(a2),-(a1)
                abcd.b  -(a2),-(a1)
                abcd.b  -(a2),-(a1)
                cmpi.l  #$360,POSTFLIGHT_PACKED_VALUE.l
                blt.s   postflight_ten_step_value_ready
                clr.l   POSTFLIGHT_PACKED_VALUE.l
postflight_ten_step_value_ready:
                lea.l   POSTFLIGHT_TABLE_BASE.l,a1
                adda.w  d1,a1
                move.w  d1,-(a7)
                bsr.w   POSTFLIGHT_SCALED_HELPER
                move.w  (a7)+,d1
                bra.s   postflight_ten_step_forward
postflight_ten_step_restore_state:
                move.w  (a7)+,d1
                move.l  (a7)+,POSTFLIGHT_PACKED_VALUE.l
                tst.l   POSTFLIGHT_PACKED_VALUE.l
                bgt.s   postflight_ten_step_value_nonpositive_done
                move.l  #$360,POSTFLIGHT_PACKED_VALUE.l
postflight_ten_step_value_nonpositive_done:
