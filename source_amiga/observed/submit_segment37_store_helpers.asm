; Byte-exact static-only segment-37 store helpers $C308D8-$C30917.

                org     $C308D8

EXTERNAL_BLIT_HELPER             equ $C53F44
CUSTOM_BLTCON0                   equ $40
CUSTOM_BLTCPTH                   equ $48
CUSTOM_BLTBPTH                   equ $50
CUSTOM_BLTDPTH                   equ $54
CUSTOM_BLTDMOD                   equ $58

submit_segment37_store_next:
                move.l  (a2)+,d4
                add.l   d1,d4
                jsr     EXTERNAL_BLIT_HELPER.l
submit_segment37_store_first:
                move.w  d2,CUSTOM_BLTCON0(a0)
                move.l  d4,CUSTOM_BLTCPTH(a0)
                move.l  d4,CUSTOM_BLTDPTH(a0)
                move.w  d6,CUSTOM_BLTDMOD(a0)
                rts

submit_segment37_store_from_table:
                move.l  (a2)+,d4
                add.l   d1,d4
                movea.l (a1)+,a3
                move.l  (a3),d0
                add.l   d7,d0
                jsr     EXTERNAL_BLIT_HELPER.l
                move.w  d2,CUSTOM_BLTCON0(a0)
                move.l  d0,CUSTOM_BLTBPTH(a0)
                move.l  d4,CUSTOM_BLTDPTH(a0)
                move.w  d6,CUSTOM_BLTDMOD(a0)
                rts

return_from_segment37_store_helper:
                rts
