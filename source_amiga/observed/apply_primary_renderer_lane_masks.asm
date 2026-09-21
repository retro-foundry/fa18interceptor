; Byte-exact static-only renderer lane-mask targets $C2F826-$C2F8CF.

                org     $C2F826

; A0-A3 hold four adjusted output pointers.  D0-D3 are the clear masks and
; D4-D7 are the set masks prepared by the shared renderer body.

primary_renderer_all_clear:
                and.w   d0,(a3)
                and.w   d1,(a2)
                and.w   d2,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_all_xor:
                eor.w   d4,(a3)
                eor.w   d5,(a2)
                eor.w   d6,(a1)
                eor.w   d7,(a0)
                rts

primary_renderer_lane0_set:
                or.w    d4,(a3)
                and.w   d1,(a2)
                and.w   d2,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_lane1_set:
                and.w   d0,(a3)
                or.w    d5,(a2)
                and.w   d2,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_lanes01_set:
                or.w    d4,(a3)
                or.w    d5,(a2)
                and.w   d2,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_lane2_set:
                and.w   d0,(a3)
                and.w   d1,(a2)
                or.w    d6,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_lanes02_set:
                or.w    d4,(a3)
                and.w   d1,(a2)
                or.w    d6,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_lanes12_set:
                and.w   d0,(a3)
                or.w    d5,(a2)
                or.w    d6,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_lanes012_set:
                or.w    d4,(a3)
                or.w    d5,(a2)
                or.w    d6,(a1)
                and.w   d3,(a0)
                rts

primary_renderer_lane3_set:
                and.w   d0,(a3)
                and.w   d1,(a2)
                and.w   d2,(a1)
                or.w    d7,(a0)
                rts

primary_renderer_lanes03_set:
                or.w    d4,(a3)
                and.w   d1,(a2)
                and.w   d2,(a1)
                or.w    d7,(a0)
                rts

primary_renderer_lanes13_set:
                and.w   d0,(a3)
                or.w    d5,(a2)
                and.w   d2,(a1)
                or.w    d7,(a0)
                rts

primary_renderer_lanes013_set:
                or.w    d4,(a3)
                or.w    d5,(a2)
                and.w   d2,(a1)
                or.w    d7,(a0)
                rts

primary_renderer_lanes23_set:
                and.w   d0,(a3)
                and.w   d1,(a2)
                or.w    d6,(a1)
                or.w    d7,(a0)
                rts

primary_renderer_lanes023_set:
                or.w    d4,(a3)
                and.w   d1,(a2)
                or.w    d6,(a1)
                or.w    d7,(a0)
                rts

primary_renderer_lanes123_set:
                and.w   d0,(a3)
                or.w    d5,(a2)
                or.w    d6,(a1)
                or.w    d7,(a0)
                rts

primary_renderer_lanes0123_set:
                or.w    d4,(a3)
                or.w    d5,(a2)
                or.w    d6,(a1)
                or.w    d7,(a0)
                rts
