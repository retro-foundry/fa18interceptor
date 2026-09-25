; Byte-exact observed candidate bound construction $C275B8-$C275CF.

                org     $C275B8

build_c275b8_candidate_bounds:
                move.w  $c(a3),d0
                ext.l   d0
                add.l   -$38(a6),d0
                move.l  $10(a3),d1
                move.w  $e(a3),d2
                ext.l   d2
                add.l   -$3c(a6),d2
