; Byte-exact packed-font render-lane selectors $C32736-$C3273F.
; Callers select one of two D6 high-word lane values, then join the common
; packed-font formatter at $C32740.

                org     $C32736

select_packed_font_render_lane_a:
                move.w  #$0F3A,d6
                bra.b   $C32740

select_packed_font_render_lane_b:
                move.w  #$0FCA,d6
