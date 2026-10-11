; Byte-exact $C304B2-$C304F9 blitter setup leaf (Hunk 36 +$10A2).
; The observed path waits for custom-register bit 6 to clear before programming
; a blitter operation.  Pointer ownership remains caller-defined.

                org     $C304B2

BLIT_SIZE_WORD                 equ $C4596E
BLIT_LANE_POINTER              equ $C45960
CUSTOM_BASE                    equ $DFF000
CUSTOM_DMACONR                 equ $002
BLTCON0                        equ $040
BLTCON1                        equ $042
BLTBPT                         equ $04C
BLTAPT                         equ $050
BLTDPT                         equ $054
BLTSIZE                        equ $058
BLITTER_BUSY_BIT               equ 6
BLTCON0_MODE_A                 equ $0D0C
BLTCON0_MODE_B                 equ $0D3C
BLTCON1_LINE                  equ $0002

setup_blitter_operation:
                move.w  BLIT_SIZE_WORD.l,d0
                move.l  BLIT_LANE_POINTER.l,d2
                move.l  d2,d1
                lea     CUSTOM_BASE.l,a0
.wait_for_blitter:
                btst.b  #BLITTER_BUSY_BIT,CUSTOM_DMACONR(a0)
                beq.s   .blitter_ready
                nop
                nop
                bra.s   .wait_for_blitter
.blitter_ready:
                move.w  #BLTCON0_MODE_A,BLTCON0(a0)
                bra.s   .write_common_setup
                move.w  #BLTCON0_MODE_B,BLTCON0(a0)
.write_common_setup:
                move.w  #BLTCON1_LINE,BLTCON1(a0)
                move.l  d2,BLTAPT(a0)
                move.l  d1,BLTBPT(a0)
                move.l  d1,BLTDPT(a0)
                move.w  d0,BLTSIZE(a0)
                rts
