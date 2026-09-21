; Byte-exact indexed-update delta clamp setup $C25E44-$C25E55.

                org     $C25E44

INDEXED_DELTA_LIMIT              equ $1FFFFFFF

prepare_indexed_update_delta_clamps:
                move.l  #INDEXED_DELTA_LIMIT,d0
                move.l  $14(a1),d2
                move.l  $1C(a1),d4
                add.l   d5,d2
                dc.w    $6D08                   ; blt.b $C25E5E
