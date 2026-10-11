; Byte-exact observed renderer-lane variant $C3003A-$C30083.
; It derives a lane index from C45986, prepares a fixed renderer request, and
; invokes C310E2 before branching on its status.  Visual primitive meaning is
; not established by this slice.

                org     $C3003A

RENDERER_LANE_INDEX             equ     $C45986
RENDERER_SHARED_LONG            equ     $C45918
RENDERER_LANE_HELPER            equ     $C310E2

submit_c300_renderer_lane_variant:
                move.w  #$C,d7
                sub.w   RENDERER_LANE_INDEX.l,d7
                blt.b   $C30038
                move.w  #$C,d7
                ; ADDI.W #$0002,D7; retain the original immediate encoding.
                dc.w    $0647,$0002
                neg.w   d7
                addi.w  #$14,d7
                sub.w   RENDERER_LANE_INDEX.l,d7
                blt.b   $C30038
                move.l  #$1990,d1
                movea.w #2,a4
                move.w  #$542,d6
                movea.w #$25,a5
                move.w  #$C,d7
                add.l   RENDERER_SHARED_LONG.l,d1
                jsr     RENDERER_LANE_HELPER.l
                blt.b   $C30038
                tst.w   d5
                beq.b   $C3008A
