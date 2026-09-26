; Byte-exact observed geometry-iteration offset accumulation $C20984-$C20995.
; It restores a three-word local tuple, adds the adjacent local triple, then
; joins the established first geometry iteration.

                org     $C20984

accumulate_geometry_iteration_offsets:
                movem.w -$58(a6),d4-d6
                add.w   -$40(a6),d4
                add.w   -$3E(a6),d5
                add.w   -$3C(a6),d6
