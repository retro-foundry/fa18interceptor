; Byte-exact observed slice of blitter_draw_line_to_enabled_planes.
; Authority: build/attract_focus_600/slow.bin at $C2FB4E-$C2FB79.
; The surrounding routine and caller convention are in analysis/routines/c2fa7e_blitter_line.md.

CUSTOM       equ $dff000
DMACONR      equ $002
BLTCON0      equ $040
BLTCON1      equ $042
BLTAFWM      equ $044
BLTAMOD      equ $064
BLTDMOD      equ $066
BLTCMOD      equ $060
BLTBDAT      equ $072
BLITTER_BUSY equ 6

    org $c2fb4e

blitter_wait_idle:
    lea     CUSTOM,a0
.wait:
    btst    #BLITTER_BUSY,DMACONR(a0)
    beq.s   .ready
    nop
    nop
    bra.s   .wait
.ready:
    move.w  a3,BLTAMOD(a0)
    move.w  #40,BLTDMOD(a0)
    move.w  #40,BLTCMOD(a0)
    move.l  d0,BLTAFWM(a0) ; D0 is -1: both A masks are all ones.
    move.w  d0,BLTBDAT(a0)
