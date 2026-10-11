; Byte-exact C2F0C6-C2F1B7 shared interpolation helpers and result gate.
; Four entry variants normalize signed components, then join at C2F186.
                org     $C2F0C6
PROJECTED_COMPONENTS          equ     $C45AC6

projected_segment_clip_d3_first:
                movem.l d0-d6,-(a7)
                movem.w 6(a1),d0-d2
                move.w  d2,d6
                sub.w   d5,d6
                sub.w   d2,d0
                neg.w   d0
                sub.w   d5,d3
                add.w   d0,d3
projected_segment_clip_d3_first_divisor:
                beq.b   projected_segment_clip_d3_first_divisor
                sub.w   d1,d4
                neg.w   d4
                muls.w  d0,d4
                divs.w  d3,d4
                sub.w   d4,d1
                muls.w  d0,d6
                divs.w  d3,d6
                sub.w   d6,d2
                move.w  d2,d0
                bra.w   projected_segment_clip_result_gate

projected_segment_clip_d3_second:
                movem.l d0-d6,-(a7)
                movem.w 6(a1),d0-d2
                neg.w   d0
                neg.w   d3
                move.w  d2,d6
                sub.w   d5,d6
                sub.w   d2,d0
                neg.w   d0
                sub.w   d5,d3
                add.w   d0,d3
projected_segment_clip_d3_second_divisor:
                beq.b   projected_segment_clip_d3_second_divisor
                sub.w   d1,d4
                neg.w   d4
                muls.w  d0,d4
                divs.w  d3,d4
                sub.w   d4,d1
                muls.w  d0,d6
                divs.w  d3,d6
                sub.w   d6,d2
                move.w  d2,d0
                neg.w   d0
                bra.w   projected_segment_clip_result_gate

projected_segment_clip_d4_first:
                movem.l d0-d6,-(a7)
                movem.w 6(a1),d0-d2
                move.w  d2,d6
                sub.w   d5,d6
                sub.w   d2,d1
                neg.w   d1
                sub.w   d5,d4
                add.w   d1,d4
projected_segment_clip_d4_first_divisor:
                beq.b   projected_segment_clip_d4_first_divisor
                sub.w   d0,d3
                neg.w   d3
                muls.w  d1,d3
                divs.w  d4,d3
                sub.w   d3,d0
                muls.w  d1,d6
                divs.w  d4,d6
                sub.w   d6,d2
                move.w  d2,d1
                bra.w   projected_segment_clip_result_gate

projected_segment_clip_d4_second:
                movem.l d0-d6,-(a7)
                movem.w 6(a1),d0-d2
                neg.w   d1
                neg.w   d4
                move.w  d2,d6
                sub.w   d5,d6
                sub.w   d2,d1
                neg.w   d1
                sub.w   d5,d4
                add.w   d1,d4
projected_segment_clip_d4_second_divisor:
                beq.b   projected_segment_clip_d4_second_divisor
                sub.w   d0,d3
                neg.w   d3
                muls.w  d1,d3
                divs.w  d4,d3
                sub.w   d3,d0
                muls.w  d1,d6
                divs.w  d4,d6
                sub.w   d6,d2
                move.w  d2,d1
                neg.w   d1

projected_segment_clip_result_gate:
                movem.w d0-d2,PROJECTED_COMPONENTS.l
                move.w  d2,d5
                blt.b   projected_segment_clip_accept
                move.w  d2,d6
                cmp.w   d5,d0
                bgt.b   projected_segment_clip_accept
                neg.w   d0
                cmp.w   d5,d0
                bgt.b   projected_segment_clip_accept
                cmp.w   d6,d1
                bgt.b   projected_segment_clip_accept
                neg.w   d1
                cmp.w   d6,d1
                ble.b   projected_segment_clip_reject
projected_segment_clip_accept:
                moveq   #1,d0
                movem.l (a7)+,d0-d6
                rts
projected_segment_clip_reject:
                moveq   #0,d0
                movem.l (a7)+,d0-d6
                rts
