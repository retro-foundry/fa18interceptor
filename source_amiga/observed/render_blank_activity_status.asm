; Byte-exact zero-selector activity-status rendering path $C329DA-$C32A43.
; It follows the zero branch from $C328A8; gameplay ownership is unresolved.

                org     $C329DA

ACTIVITY_STATUS_SCRATCH          equ     $C457FA
ACTIVITY_RENDER_LANE             equ     $C45986
ACTIVITY_RENDER_OFFSET           equ     $C45918
ACTIVITY_GLYPH_RENDERER          equ     $C32794
ACTIVITY_TRANSFORM_FOLLOWUP      equ     $C32A44

render_blank_activity_status:
                move.b  #' ',(a2)
                move.b  #' ',1(a2)
                move.b  #' ',2(a2)
                ; LEA $C3191C(PC),A1; retain original PC-relative encoding.
                dc.w    $43FA,$EF30
                move.w  d7,d2
                lea.l   $1CC6.w,a4
                lea.l   $6.w,a5
                move.w  #4,d5
                move.w  #$FCA,d6
                swap    d6
                move.w  ACTIVITY_RENDER_LANE.l,d6
                move.l  ACTIVITY_RENDER_OFFSET.l,d7
                moveq   #2,d0
                bsr.w   ACTIVITY_GLYPH_RENDERER
                lea.l   ACTIVITY_STATUS_SCRATCH.l,a2
                ; LEA $C3191C(PC),A1; retain original PC-relative encoding.
                dc.w    $43FA,$EF00
                move.w  d7,d2
                lea.l   $1B86.w,a4
                lea.l   $6.w,a5
                move.w  #4,d5
                moveq   #2,d0
                move.w  #$FCA,d6
                swap    d6
                move.w  ACTIVITY_RENDER_LANE.l,d6
                move.l  ACTIVITY_RENDER_OFFSET.l,d7
                bra.w   ACTIVITY_GLYPH_RENDERER

