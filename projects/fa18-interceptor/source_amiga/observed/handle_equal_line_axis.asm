; Byte-exact reconstruction of $C2FA70-$C2FA77 (Hunk 36 +$5E0).
; Equal-axis fallback entering prepare_blitter_line_parameters at $C2FA9C.

                org     $C2FA70

prepare_blitter_line_forward_axis equ $C2FA9C

handle_equal_line_axis:
                addq.w  #1,d1
                addq.w  #1,d3
                clr.w   d5
                bra.b   prepare_blitter_line_forward_axis
