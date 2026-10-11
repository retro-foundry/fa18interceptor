; Byte-exact observed renderer blit submission $C30CDE-$C30D21.

                org     $C30CDE

EXTERNAL_BLIT_HELPER             equ     $C53F44
CUSTOM_BASE                      equ     $DFF000

submit_c30cde_renderer_blit:
                add.w   d7,d5
                add.l   d1,d4
                sub.w   d5,d6
                add.w   d5,d5
                add.w   a5,d5
                lea     CUSTOM_BASE.l,a0
                jsr     EXTERNAL_BLIT_HELPER.l
