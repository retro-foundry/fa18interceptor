; Byte-exact renderer lane stage $C2FF58-$C30037.

                org     $C2FF58

RENDERER_ENABLE_WORD            equ $C456E8
RENDERER_SCALE_WORD             equ $C456EA
RENDERER_STAGE_FLAG             equ $C456EC
RENDERER_LANE_ENABLES           equ $C456E7
RENDERER_LINE_CONTROL           equ $C45956
CUSTOM_DMACON                   equ $DFF096
DMA_MASTER_AND_BLITTER          equ $0400
RENDERER_STAGE_PRELUDE          equ $C3040C
RENDERER_LANE_HELPER            equ $C30466
RENDERER_BLITTER_SETUP          equ $C304B2

run_renderer_lane_stage:
                tst.w   RENDERER_ENABLE_WORD.l
                blt.s   .lane0
                move.w  RENDERER_STAGE_FLAG.l,d5
                beq.s   .lane0
                bsr.w   RENDERER_STAGE_PRELUDE
                bra.w   .finish_lanes
.lane0:
                btst    #0,RENDERER_LANE_ENABLES.l
                beq.s   .lane0_disabled
                moveq   #$C,d0
                move.w  RENDERER_SCALE_WORD.l,d4
                move.w  RENDERER_ENABLE_WORD.l,d3
                bge.s   .lane0_ready
                move.w  RENDERER_LINE_CONTROL.l,d3
.lane0_ready:
                bsr.w   RENDERER_LANE_HELPER
                bra.s   .lane1
.lane0_disabled:
                lsr.w   RENDERER_LINE_CONTROL.l
.lane1:
                btst    #1,RENDERER_LANE_ENABLES.l
                beq.s   .lane1_disabled
                moveq   #8,d0
                move.w  RENDERER_ENABLE_WORD.l,d3
                blt.s   .lane1_negative
                move.w  RENDERER_SCALE_WORD.l,d4
                asr.w   #1,d3
                asr.w   #1,d4
                bra.s   .lane1_ready
.lane1_negative:
                move.w  RENDERER_LINE_CONTROL.l,d3
.lane1_ready:
                bsr.w   RENDERER_LANE_HELPER
                bra.s   .lane2
.lane1_disabled:
                lsr.w   RENDERER_LINE_CONTROL.l
.lane2:
                btst    #2,RENDERER_LANE_ENABLES.l
                beq.s   .lane2_disabled
                moveq   #4,d0
                move.w  RENDERER_ENABLE_WORD.l,d3
                blt.s   .lane2_negative
                move.w  RENDERER_SCALE_WORD.l,d4
                asr.w   #2,d3
                asr.w   #2,d4
                bra.s   .lane2_ready
.lane2_negative:
                move.w  RENDERER_LINE_CONTROL.l,d3
.lane2_ready:
                bsr.w   RENDERER_LANE_HELPER
                bra.s   .lane3
.lane2_disabled:
                lsr.w   RENDERER_LINE_CONTROL.l
.lane3:
                btst    #3,RENDERER_LANE_ENABLES.l
                beq.s   .finish_lanes
                moveq   #0,d0
                move.w  RENDERER_ENABLE_WORD.l,d3
                blt.s   .lane3_negative
                move.w  RENDERER_SCALE_WORD.l,d4
                asr.w   #3,d3
                asr.w   #3,d4
                bra.s   .lane3_ready
.lane3_negative:
                move.w  RENDERER_LINE_CONTROL.l,d3
.lane3_ready:
                bsr.w   RENDERER_LANE_HELPER
.finish_lanes:
                bsr.w   RENDERER_BLITTER_SETUP
                move.w  #DMA_MASTER_AND_BLITTER,CUSTOM_DMACON.l
                rts
