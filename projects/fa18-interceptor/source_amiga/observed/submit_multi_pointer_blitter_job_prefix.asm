; Byte-exact runtime-backed submission prefix $C30E06-$C30E43.
; The following status branch is intentionally outside this prefix.

                org     $C30E06

BLTCON0         equ     $040
BLTCON1         equ     $042
BLTAFWM         equ     $044
BLTALWM         equ     $046
BLTCPT          equ     $048
BLTBPT          equ     $04C
BLTDPT          equ     $054
BLTAPT          equ     $050
BLTCMOD         equ     $060
BLTBMOD         equ     $062
BLTAMOD         equ     $064
BLTDMOD         equ     $066
BLTSIZE         equ     $058

submit_multi_pointer_blitter_job_prefix:
                move.w  d2,BLTCON0(a0)
                move.w  #0,BLTCON1(a0)
                move.w  #$FFFF,BLTAFWM(a0)
                move.w  #$FFFF,BLTALWM(a0)
                move.w  d5,BLTAMOD(a0)
                move.w  d5,BLTBMOD(a0)
                subq.w  #1,d5
                add.w   a5,d5
                move.w  d5,BLTCMOD(a0)
                move.w  d5,BLTDMOD(a0)
                move.l  d0,BLTAPT(a0)
                move.l  d3,BLTBPT(a0)
                move.l  d4,BLTCPT(a0)
                move.l  d4,BLTDPT(a0)
                move.w  d6,BLTSIZE(a0)
