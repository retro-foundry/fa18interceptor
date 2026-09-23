; Byte-exact map packet relative-offset selector $C2AD80-$C2AE59.
; A2 indexes signed byte-pair controls at C29F00; A1 is the selected packet
; directory base from the caller's frame.  The C2ADF8 table is consumed by
; apply_map_control_detail_fields at C2AE5A.

                org     $C2AD80

MAP_SELECTOR_BYTE_PAIR_TABLE equ     $C29F00
MAP_STATUS_WORD              equ     $C4599E
MAP_SELECTOR_RECORD_BASE     equ     -$34
MAP_SELECTOR_ROW_MIN         equ     -$2C
MAP_SELECTOR_ROW_MAX         equ     -$30
MAP_SELECTOR_COLUMN_MIN      equ     -$2A
MAP_SELECTOR_COLUMN_MAX      equ     -$2E
MAP_SELECTOR_MODE            equ     -$3E

select_map_packet_relative_offset:
                lea.l   MAP_SELECTOR_BYTE_PAIR_TABLE(pc),a2
                add.w   d2,d2
                adda.w  d2,a2
                lea.l   (a2),a1
                move.b  (a1)+,d3
                ext.w   d3
                move.w  MAP_SELECTOR_ROW_MIN(a6),d0
                add.w   d3,d0
                blt.w   $C2AD00
                cmp.w   MAP_SELECTOR_ROW_MAX(a6),d0
                bgt.w   $C2AD00
                move.b  (a1),d3
                ext.w   d3
                move.w  MAP_SELECTOR_COLUMN_MIN(a6),d1
                add.w   d3,d1
                blt.w   $C2AD00
                cmp.w   MAP_SELECTOR_COLUMN_MAX(a6),d1
                bgt.w   $C2AD00
                movea.l MAP_SELECTOR_RECORD_BASE(a6),a1
                tst.w   MAP_SELECTOR_MODE(a6)
                bne.b   .wide_row_stride
                add.w   d0,d0
                lsl.w   #4,d1
                bra.b   .read_relative_offset
.wide_row_stride:
                add.w   d0,d0
                lsl.w   #6,d1
.read_relative_offset:
                add.w   d1,d0
                lea.l   (a1),a3
                move.w  (a1,d0.w),d0
                ble.b   .invalid_relative_offset
                adda.w  d0,a3
                tst.w   (a3)
                bge.b   $C2AE1C
                tst.b   $C457B0.l
                bne.b   $C2AE1C
                bra.w   $C2AD00
.invalid_relative_offset:
                move.w  #$40,MAP_STATUS_WORD.l
                jsr     $C06C02.l
                bra.w   $C2AD00

; 18 words; C2AE5A limits its index to 0..17 before reading this table.
map_detail_limit_lookup:
                dc.w    $0084,$0088,$0090,$0098,$009C,$00A0,$00A2,$00A4
                dc.w    $00A6,$00A8,$00B0,$00C0,$00F0,$0100,$0108,$0110
                dc.w    $0120,$0120

prepare_map_packet_projection_terms:
                move.l  $0000.w(a4),d0
                move.l  $8(a4),d1
                tst.w   MAP_SELECTOR_MODE(a6)
                bne.b   .wide_coordinate_mask
                andi.l  #$0FFFFFFF,d0
                andi.l  #$0FFFFFFF,d1
                bra.b   .coordinate_masked
.wide_coordinate_mask:
                andi.l  #$00FFFFFF,d0
                andi.l  #$00FFFFFF,d1
.coordinate_masked:
                neg.l   d0
                neg.l   d1
                move.b  (a2)+,d3
                ext.w   d3
                move.b  (a2),d4
                ext.w   d4
                asl.w   #8,d3
                asl.w   #8,d4
                swap    d3
                swap    d4
                clr.w   d3
