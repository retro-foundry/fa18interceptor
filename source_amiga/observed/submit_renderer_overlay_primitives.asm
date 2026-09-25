; Byte-exact observed renderer overlay submission tail $C300CE-$C30199.
; This programs a blitter lane then emits fixed-coordinate primitive requests.
; The on-screen object/content represented by the requests is not identified.

                org     $C300CE

RENDERER_CONTROL_LONG          equ     $C456E6
RENDERER_SELECTOR_WORD         equ     $C45954
RENDERER_X_OFFSET               equ     $C45988
RENDERER_Y_OFFSET               equ     $C458D8
CUSTOM_BLTCON0                 equ     $40
CUSTOM_BLTCON1                 equ     $42
CUSTOM_BLTCPTH                 equ     $44
CUSTOM_BLTCPTH_LONG            equ     $48
CUSTOM_BLTBPTH                 equ     $4C
CUSTOM_BLTAPTH                 equ     $54
CUSTOM_BLTAFWM                 equ $60
CUSTOM_BLTADAT                 equ $62
CUSTOM_BLTCDAT                 equ $66
CUSTOM_BLTDMOD                 equ $74
CUSTOM_BLTSIZE                 equ $58

submit_renderer_overlay_primitives:
                move.w  d2,CUSTOM_BLTCON0(a0)
                move.w  #0,CUSTOM_BLTCON1(a0)
                move.w  #$FFFF,CUSTOM_BLTDMOD(a0)
                move.w  d0,CUSTOM_BLTCPTH(a0)
                move.w  d3,CUSTOM_BLTCPTH+2(a0)
                move.w  #1,CUSTOM_BLTADAT(a0)
                move.w  d5,CUSTOM_BLTAFWM(a0)
                move.w  d5,CUSTOM_BLTCDAT(a0)
                move.l  d7,CUSTOM_BLTBPTH(a0)
                move.l  d4,CUSTOM_BLTCPTH_LONG(a0)
                move.l  d4,CUSTOM_BLTAPTH(a0)
                move.w  d6,CUSTOM_BLTSIZE(a0)
                bsr.w   $C3019C
                move.l  #$0007FFFF,RENDERER_CONTROL_LONG.l
                move.w  #$CC,d0
                add.w   RENDERER_X_OFFSET.l,d0
                move.w  #$AC,d1
                add.w   RENDERER_Y_OFFSET.l,d1
                move.w  d0,d2
                addi.w  #$A,d2
                move.w  d1,d3
                move.w  #2,RENDERER_SELECTOR_WORD.l
                bsr.w   $C2FA78
                move.w  #$D1,d0
                move.w  #$AB,d1
                bsr.w   $C2F5C0
                move.w  #$AC,d1
                bsr.w   $C2F5F4
                move.w  #$D,RENDERER_SELECTOR_WORD.l
                move.w  #$CE,d0
                move.w  #$A5,d1
                bsr.w   $C2F5C0
                subq.w  #2,d0
                addq.w  #1,d1
                bsr.w   $C2F5D4
                subq.w  #1,d0
                addq.w  #1,d1
                bsr.w   $C2F5D4
                subq.w  #2,d0
                addq.w  #3,d1
                bsr.w   $C2F5D4
                addi.w  #$11,d0
                addq.w  #1,d1
                bsr.w   $C2F5D4
                addq.w  #1,d1
                bsr.w   $C2F5D4
                addq.w  #1,d1
                bsr.w   $C2F5D4
                subq.w  #1,d0
                addq.w  #2,d1
                bsr.w   $C2F5D4
                rts
