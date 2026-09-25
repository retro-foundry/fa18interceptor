; Byte-exact renderer pair-bound preparation $C3019C-$C301F5.
; It converts source pair values into the C4B390 list, invokes its bounded
; submitter, and falls back to two renderer lane helpers when needed.

                org     $C3019C

PAIR_SOURCE_LIST                equ     $C4B432
PAIR_BOUND_LIST                 equ     $C4B390
PAIR_HORIZONTAL_OFFSET          equ     $C45988
PAIR_VERTICAL_OFFSET            equ     $C458D8
PAIR_BOUND_SUBMITTER            equ     $C301F0
ADJUSTED_LANE_BLIT              equ     $C304FA
BLIT_SETUP                      equ     $C304B2

build_renderer_pair_bounds:
                lea     PAIR_SOURCE_LIST.l,a0
                lea     PAIR_BOUND_LIST.l,a4
                move.w  (a0)+,d7
                ble.b   $C3019A
                move.w  d7,(a4)+
                subq.w  #1,d7
                move.w  (a0)+,d0
                mulu.w  #$18,d0
                asr.l   #8,d0
                addi.w  #$C1,d0
                add.w   PAIR_HORIZONTAL_OFFSET.l,d0
                move.w  d0,(a4)+
                move.w  (a0)+,d0
                mulu.w  #$1F,d0
                asr.l   #8,d0
                addi.w  #$A2,d0
                add.w   PAIR_VERTICAL_OFFSET.l,d0
                move.w  d0,(a4)+
                dbra    d7,$C301B0
                bsr.w   PAIR_BOUND_SUBMITTER
                bne.b   $C3019A
                moveq   #4,d0
                moveq   #1,d3
                bsr.w   ADJUSTED_LANE_BLIT
                bsr.w   BLIT_SETUP
                rts
                movea.w #$C7,a4
                bra.b   $C301FC
