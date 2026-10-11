; Byte-exact runtime-backed blitter submission leaf $C30CF4-$C30D21.
; Callers prepare D0,D2-D6 and A0.  C and D use the same D4 pointer.

                org     $C30CF4

BLTCON0         equ     $040
BLTCON1         equ     $042
BLTAFWM         equ     $044
BLTALWM         equ     $046
BLTCPT          equ     $048
BLTDPT          equ     $054
BLTSIZE         equ     $058
BLTCMOD         equ     $060
BLTDMOD         equ     $066
BLTADAT         equ     $074

submit_single_pointer_blitter_job:
                move.w  d2,BLTCON0(a0)
                move.w  #0,BLTCON1(a0)
                move.w  #$FFFF,BLTADAT(a0)
                move.w  d0,BLTAFWM(a0)
                move.w  d3,BLTALWM(a0)
                move.w  d5,BLTCMOD(a0)
                move.w  d5,BLTDMOD(a0)
                move.l  d4,BLTCPT(a0)
                move.l  d4,BLTDPT(a0)
                move.w  d6,BLTSIZE(a0)
                rts
