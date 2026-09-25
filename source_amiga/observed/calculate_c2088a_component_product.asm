; Byte-exact observed accumulated product and scaling $C2088A-$C20899.

                org     $C2088A

calculate_c2088a_component_product:
                muls.w  d5,d0
                muls.w  d6,d1
                muls.w  d7,d2
                add.l   d0,d2
                add.l   d1,d2
                bge.b   .nonnegative
                neg.l   d2
.nonnegative:
                asr.l   #4,d2
