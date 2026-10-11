; Byte-exact static-only alternate helper loops $C336FA-$C33779.
                org     $C336FA
POSTFLIGHT_PACKED_VALUE         equ $C45B22
POSTFLIGHT_BCD_LEFT             equ $C45B26
POSTFLIGHT_BCD_RIGHT            equ $C45B2A
POSTFLIGHT_SCALED_HELPER        equ $C33F70
run_postflight_alternate_helper_loops:
                move.l  #5,POSTFLIGHT_BCD_LEFT.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,-(a7)
                move.l  d5,-(a7)
postflight_alternate_negative_pass:
                subi.l  #$258,d5
                cmpi.l  #$AF0,d5
                blt.s   postflight_alternate_restore_state
                lea.l   POSTFLIGHT_BCD_LEFT.l,a1
                lea.l   POSTFLIGHT_BCD_RIGHT.l,a2
                addi.w  #0,d0
                abcd.b  -(a2),-(a1)
                abcd.b  -(a2),-(a1)
                abcd.b  -(a2),-(a1)
                move.l  d5,-(a7)
                bsr.w   POSTFLIGHT_SCALED_HELPER
                move.l  (a7)+,d5
                bra.s   postflight_alternate_negative_pass
postflight_alternate_restore_state:
                move.l  (a7)+,d5
                move.l  (a7)+,POSTFLIGHT_PACKED_VALUE.l
postflight_alternate_positive_pass:
                tst.l   POSTFLIGHT_PACKED_VALUE.l
                beq.w   postflight_alternate_loops_done
                addi.l  #$258,d5
                cmpi.l  #$1130,d5
                bgt.s   postflight_alternate_loops_done
                lea.l   POSTFLIGHT_BCD_LEFT.l,a1
                lea.l   POSTFLIGHT_BCD_RIGHT.l,a2
                addi.w  #0,d0
                sbcd.b  -(a2),-(a1)
                sbcd.b  -(a2),-(a1)
                sbcd.b  -(a2),-(a1)
                move.l  d5,-(a7)
                bsr.w   POSTFLIGHT_SCALED_HELPER
                move.l  (a7)+,d5
                bra.s   postflight_alternate_positive_pass
postflight_alternate_loops_done:
