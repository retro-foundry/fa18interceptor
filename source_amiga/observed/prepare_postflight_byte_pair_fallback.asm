; Byte-exact C34696-C346BD alternate window setup for byte-pair fallback.
                org     $C34696
RENDERER_X_OFFSET                equ     $C45988

prepare_postflight_byte_pair_fallback:
                move.w  #$55,d0
                add.w   RENDERER_X_OFFSET.l,d0
                move.w  d0,-6(a6)
                move.w  #$E9,d0
                add.w   RENDERER_X_OFFSET.l,d0
                move.w  d0,-8(a6)
                move.w  #$2D,-10(a6)
                move.w  #$90,-12(a6)
