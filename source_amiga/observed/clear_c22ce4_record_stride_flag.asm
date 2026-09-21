; Byte-exact strided record flag clear $C22CE4-$C22D2D.

                org     $C22CE4

RECORD_STRIDE_TABLE             equ $C46184

clear_c22ce4_record_stride_flag:
                lea     RECORD_STRIDE_TABLE.l,a0
                move.w  #-$2,d0
                and.w   d0,$2(a0)
                and.w   d0,$202(a0)
                and.w   d0,$402(a0)
                and.w   d0,$602(a0)
                and.w   d0,$802(a0)
                and.w   d0,$A02(a0)
                and.w   d0,$C02(a0)
                and.w   d0,$E02(a0)
                and.w   d0,$1002(a0)
                and.w   d0,$1202(a0)
                and.w   d0,$1402(a0)
                and.w   d0,$1602(a0)
                and.w   d0,$1802(a0)
                and.w   d0,$1A02(a0)
                and.w   d0,$1C02(a0)
