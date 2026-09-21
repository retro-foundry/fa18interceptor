; Byte-exact $C30668-$C306B3 blitter submission leaf (Hunk 36 +$1258).
; Callers prepare D1-D7 and A0; this leaf waits then writes the complete job.

                org     $C30668

CUSTOM_DMACONR          equ $002
BLTCON0                 equ $040
BLTCON1                 equ $042
BLTAFWM                 equ $044
BLTALWM                 equ $046
BLTCPT                  equ $048
BLTBPT                  equ $052
BLTAPT                  equ $054
BLTSIZE                 equ $058
BLTCMOD                 equ $060
BLTBMOD                 equ $062
BLTAMOD                 equ $064
BLTDMOD                 equ $066
BLTBDAT                 equ $072
BLTADAT                 equ $074
BLITTER_BUSY_BIT        equ 6
BLIT_DATA_MASK          equ $FFFF
BLIT_LINE_DATA          equ $8000
BLIT_MODULO             equ $0028

submit_prepared_blitter_job:
                moveq   #-1,d0
.wait_for_blitter:
                btst.b  #BLITTER_BUSY_BIT,CUSTOM_DMACONR(a0)
                beq.b   .submit
                nop
                nop
                bra.b   .wait_for_blitter
.submit:
                move.w  d6,BLTCON0(a0)
                move.w  d1,BLTCON1(a0)
                move.w  d5,BLTAMOD(a0)
                move.w  #BLIT_MODULO,BLTDMOD(a0)
                move.w  #BLIT_MODULO,BLTCMOD(a0)
                move.w  d2,BLTBPT(a0)
                move.l  d7,BLTAPT(a0)
                move.l  d7,BLTCPT(a0)
                move.l  d0,BLTAFWM(a0)
                move.w  d0,BLTBDAT(a0)
                move.w  #BLIT_LINE_DATA,BLTADAT(a0)
                move.w  d3,BLTBMOD(a0)
                move.w  d4,BLTSIZE(a0)
                rts


