; Byte-exact observed product comparison and zero return $C208C2-$C208CF.

                org     $C208C2

return_c208c2_component_product:
                add.w   d0,d0
                move.w  $0(a3,d0.w),d1
                cmp.w   d1,d2
                blt.b   $C208D0
                moveq   #0,d0
                rts
