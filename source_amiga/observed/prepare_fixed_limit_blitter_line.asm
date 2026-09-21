; Byte-exact static-only alternate blitter-line entry $C2FA78-$C2FA7D.

                org     $C2FA78

FIXED_LINE_LIMIT                equ $00C7
BLITTER_LINE_SETUP_JOIN         equ $C2FA84

prepare_fixed_limit_blitter_line:
                movea.w #FIXED_LINE_LIMIT,a2
                bra.s   BLITTER_LINE_SETUP_JOIN
