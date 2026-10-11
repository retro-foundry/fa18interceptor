; Byte-exact renderer pair finalization and blitter submission $C302E6-$C3040B.
; Observed in the attract and flight renderer packets.  Field ownership and
; geometric interpretation remain unresolved; names below retain literal roles.

                org     $C302E6

RENDERER_FALLBACK_EXIT         equ     $C3029E
RENDERER_PAIR_WORDS            equ     $C4597C
RENDERER_SOURCE_WORD           equ     $C45954
RENDERER_DESTINATION_WORD      equ     $C45956
RENDERER_PAIR_TABLE            equ     $C4B390
RENDERER_PAIR_COUNT            equ     $C45970
RENDERER_AUX_PAIR_WORDS        equ     $C4B392
RENDERER_VERTICAL_VALUE        equ     $C45982
RENDERER_HORIZONTAL_VALUE      equ     $C45980
RENDERER_BASE_LONG             equ     $C456E2
RENDERER_OFFSET_LONG           equ     $C45968
RENDERER_LANE_LONG             equ     $C45960
RENDERER_LANE_COPY             equ     $C45964
RENDERER_BLIT_SIZE             equ     $C4596E
CUSTOM_BASE                    equ     $DFF000
CUSTOM_DMACONR                 equ 2
CUSTOM_BLTCON0                 equ $40
CUSTOM_BLTCON1                 equ $42
CUSTOM_BLTCPTH                 equ $44
CUSTOM_BLTAPTH                 equ $50
CUSTOM_BLTDPTH                 equ $54
CUSTOM_BLTSIZE                 equ $58
CUSTOM_BLTADAT                 equ $62
CUSTOM_BLTBDAT                 equ $64
CUSTOM_BLTCDAT                 equ $66

finalize_renderer_pair_blit:
                cmpi.w  #1,d6
                ble.s   RENDERER_FALLBACK_EXIT
                subq.w  #1,d0
                bge.s   .nonnegative_index
                clr.w   d0
.nonnegative_index:
                exg     d1,d2
                movem.w d0-d3,RENDERER_PAIR_WORDS.l
                move.w  RENDERER_SOURCE_WORD.l,RENDERER_DESTINATION_WORD.l
                lea     RENDERER_PAIR_TABLE.l,a2
                lea     RENDERER_PAIR_COUNT.l,a3
                move.w  (a2)+,(a3)
                subq.w  #1,(a3)
                lea     CUSTOM_BASE.l,a0
.submit_pair_loop:
                movem.w (a2),d0-d3
                addq.w  #4,a2
                move.w  a4,-(sp)
                bsr.w   $C305AA
                movea.w (sp)+,a4
                subq.w  #1,(a3)
                bgt.s   .submit_pair_loop
                move.w  (a2)+,d0
                move.w  (a2),d1
                movem.w RENDERER_AUX_PAIR_WORDS.l,d2-d3
                move.w  a4,-(sp)
                bsr.w   $C305AA
                movea.w (sp)+,a4
                movem.w RENDERER_PAIR_WORDS.l,d1/d3
                asr.w   #4,d3
                move.w  d3,d2
                asr.w   #4,d1
                sub.w   d1,d3
                move.w  d3,d6
                add.w   d3,d3
                neg.w   d3
                addi.w  #$27,d3
                move.w  RENDERER_VERTICAL_VALUE.l,d5
                move.w  d5,d1
                move.w  d5,d7
                asl.w   #3,d1
                move.w  d1,d0
                add.w   d0,d0
                add.w   d0,d0
                add.w   d0,d1
                add.w   d2,d2
                ext.l   d2
                add.l   d2,d1
                move.l  d1,d2
                cmp.w   a4,d7
                ble.s   .no_pair_delta
                sub.w   a4,d7
                move.w  d7,d0
                asl.w   #3,d7
                move.w  d7,d4
                add.w   d4,d4
                add.w   d4,d4
                add.w   d4,d7
                ext.l   d7
                sub.l   d7,d1
                bra.s   .store_lanes
.no_pair_delta:
                moveq   #0,d0
                moveq   #0,d7
.store_lanes:
                move.l  d1,RENDERER_OFFSET_LONG.l
                move.l  d2,d1
                add.l   RENDERER_BASE_LONG.l,d1
                sub.l   d7,d1
                move.l  d1,RENDERER_LANE_LONG.l
                move.l  d1,d2
                move.l  d1,RENDERER_LANE_COPY.l
                sub.w   RENDERER_HORIZONTAL_VALUE.l,d5
                addq.w  #1,d5
                move.w  d5,d7
                addq.w  #1,d6
                sub.w   d0,d7
                lsl.w   #6,d7
                add.w   d6,d7
                move.w  d7,RENDERER_BLIT_SIZE.l
                moveq   #-1,d0
                lea     CUSTOM_BASE.l,a0
.wait_for_blitter:
                btst    #6,CUSTOM_DMACONR(a0)
                beq.s   .submit_blit
                nop
                nop
                bra.s   .wait_for_blitter
.submit_blit:
                move.w  #$09F0,CUSTOM_BLTCON0(a0)
                move.w  #$000A,CUSTOM_BLTCON1(a0)
                move.l  d2,CUSTOM_BLTAPTH(a0)
                move.l  d2,CUSTOM_BLTDPTH(a0)
                move.l  d0,CUSTOM_BLTCPTH(a0)
                move.w  d3,CUSTOM_BLTBDAT(a0)
                move.w  d3,CUSTOM_BLTADAT(a0)
                move.w  d3,CUSTOM_BLTCDAT(a0)
                move.w  d7,CUSTOM_BLTSIZE(a0)
                moveq   #0,d0
                rts
