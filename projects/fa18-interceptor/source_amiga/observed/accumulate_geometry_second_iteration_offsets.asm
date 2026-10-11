; Byte-exact observed second geometry-iteration offset accumulation
; $C209E2-$C209F3.  It restores and advances the same three-word local tuple
; before the established second geometry iteration.

                org     $C209E2

accumulate_geometry_second_iteration_offsets:
                movem.w -$58(a6),d4-d6
                add.w   -$40(a6),d4
                add.w   -$3E(a6),d5
                add.w   -$3C(a6),d6
