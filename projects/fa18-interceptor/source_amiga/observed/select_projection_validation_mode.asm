; Byte-exact $C2EC90-$C2EC9B entry pair before the shared validation prefix.
; $C2EC90 is exercised by the run041 C35A98 interval; $C2EC94 is static-only.
                org $C2EC90
PROJECTION_MODE_WORD equ $C45AB8
select_fixed_projection_mode:
 moveq #-5,d7
 bra.b $C2ECAA
select_record_projection_mode:
 move.w PROJECTION_MODE_WORD.l,d7
 bra.b $C2ECAA
