; Byte-exact static-only reverse-step tail $C3391A-$C3395D.
                org     $C3391A
POSTFLIGHT_BCD_LEFT             equ $C45B26
POSTFLIGHT_BCD_RIGHT            equ $C45B2A
POSTFLIGHT_VERTICAL_OFFSET      equ $C458D8
POSTFLIGHT_TABLE_BASE           equ $C33A16
POSTFLIGHT_SCALED_HELPER        equ $C33F54
POSTFLIGHT_FINAL_HELPER         equ $C34068
POSTFLIGHT_REVERSE_STATE_TEST   equ $C33908
run_postflight_reverse_step_tail:
postflight_ten_step_reverse:
                subi.w  #$A0,d1
                cmpi.w  #$FF50,d1
                blt.s   postflight_reverse_final_call
                lea.l   POSTFLIGHT_BCD_LEFT.l,a1
                lea.l   POSTFLIGHT_BCD_RIGHT.l,a2
                addi.w  #0,d0
                sbcd.b  -(a2),-(a1)
                sbcd.b  -(a2),-(a1)
                sbcd.b  -(a2),-(a1)
                lea.l   POSTFLIGHT_TABLE_BASE.l,a1
                adda.w  d1,a1
                move.w  d1,-(a7)
                bsr.w   POSTFLIGHT_SCALED_HELPER
                move.w  (a7)+,d1
                bra.s   POSTFLIGHT_REVERSE_STATE_TEST
postflight_reverse_final_call:
                dc.w    $223C
                dc.l    $3D
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                dc.w    $6100,$070C ; bsr.w POSTFLIGHT_FINAL_HELPER
                rts
