; Byte-exact table-assisted three-component scalar $C1D974-$C1D9D7.
; Inputs: component words D2/D3/D4. Output: capped scalar in D1 and C45B40.

                org     $C1D974

MAGNITUDE_RATIO_TABLE          equ     $C1D9D8
MAGNITUDE_OUTPUT               equ     $C45B40

calculate_c1d974_component_magnitude:
                movem.l d5/a0,-(sp)
                moveq   #$e,d5
                lea     MAGNITUDE_RATIO_TABLE.l,a0
                cmp.w   d2,d3
                ble.s   .first_pair_ordered
                exg     d2,d3
.first_pair_ordered:
                ext.l   d3
                beq.s   .first_ratio_ready
                asl.l   #8,d3
                tst.w   d2
                bne.s   .divide_first_ratio
                moveq   #0,d3
                bra.s   .first_ratio_ready
.divide_first_ratio:
                divu.w  d2,d3
                add.w   d3,d3
.first_ratio_ready:
                ; Preserve the observed indexed zero-displacement encoding.
                dc.w    $3630,$3000
                mulu.w  d3,d2
                ext.l   d4
                asl.l   d5,d4
                cmp.l   d2,d4
                ble.s   .second_pair_ordered
                exg     d2,d4
.second_pair_ordered:
                asr.l   d5,d2
                tst.w   d2
                bne.s   .divide_second_ratio
                moveq   #0,d4
                bra.s   .second_ratio_ready
.divide_second_ratio:
                divu.w  d2,d4
                asr.w   #6,d4
                add.w   d4,d4
.second_ratio_ready:
                ; Preserve the observed indexed zero-displacement encoding.
                dc.w    $3230,$4000
                mulu.w  d2,d1
                asr.l   d5,d1
                movem.l (sp)+,d5/a0
                cmpi.l  #$7fff,d1
                ble.s   .store_result
                move.w  #$7fff,d1
.store_result:
                move.w  d1,MAGNITUDE_OUTPUT.l
                rts
