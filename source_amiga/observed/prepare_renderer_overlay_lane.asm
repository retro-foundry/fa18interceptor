; Byte-exact observed renderer overlay-lane setup $C30098-$C300C7.
; It prepares lane arithmetic, resolves the active renderer pointer table, and
; waits on DMACONR bit 6 before the existing overlay submission continuation.

                org     $C30098

RENDERER_POINTER_BLOCK          equ     $C456B6
CUSTOM_BASE                      equ     $DFF000
DMACONR_OFFSET                   equ     $0002

prepare_renderer_overlay_lane:
                move.w  #$0FFF,d0
                add.w   d7,d5
                move.l  #$12A88,d7
                movea.l RENDERER_POINTER_BLOCK.l,a2
                move.l  $4(a2),d4
                add.l   d1,d4
                sub.w   d5,d6
                add.w   d5,d5
                add.w   a5,d5
                lea     CUSTOM_BASE.l,a0
                move.w  #$722,d2
                btst    #6,DMACONR_OFFSET(a0)
                beq.b   $C300CE
