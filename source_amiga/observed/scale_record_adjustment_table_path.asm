; Byte-exact observed table-scaling continuation $C2DE5C-$C2DE77.

                org     $C2DE5C

RECORD_ADJUSTMENT_SCALE_TABLE   equ     $C2DEC2

scale_record_adjustment_table_path:
                cmpi.w  #$e10,d1
                bgt.b   $C2DE7A
                asr.w   #8,d1
                add.w   d1,d1
                lea.l   RECORD_ADJUSTMENT_SCALE_TABLE.l,a0
                dc.w    $C7F0,$1000 ; muls.w 0(a0,d1.w),d3; retain original EA
                dc.w    $CBF0,$1000 ; muls.w 0(a0,d1.w),d5; retain original EA
                asr.l   #8,d3
                asr.l   #8,d5
