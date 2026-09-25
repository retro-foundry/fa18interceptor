; Byte-exact observed map-packet column-table selection $C2ACAA-$C2ACE9.
; A byte selector chooses aligned offsets into three local table banks; the
; current signed workspace metric then selects an additional a0 displacement.

                org     $C2ACAA

MAP_PACKET_TABLE_A             equ     $C2A7DC
MAP_PACKET_TABLE_B             equ     $C2A4DC
MAP_PACKET_TABLE_C             equ     $C2A0DC
MAP_PACKET_TABLE_SELECTOR      equ     $C45850

select_map_packet_column_tables:
                lea     MAP_PACKET_TABLE_A(pc),a0
                lea     MAP_PACKET_TABLE_B(pc),a1
                lea     MAP_PACKET_TABLE_C(pc),a2
                move.b  MAP_PACKET_TABLE_SELECTOR.l,d3
                ext.w   d3
                asl.w   #4,d3
                move.w  d3,d4
                add.w   d3,d3
                adda.w  d3,a0
                add.w   d3,d4
                adda.w  d4,a1
                add.w   d3,d3
                adda.w  d3,a2
                cmpi.l  #$4800,-$28(a6)
                bgt.b   $C2ACFA
                cmpi.l  #$3000,-$28(a6)
                bgt.b   $C2ACEA
                add.w   d2,d2
                add.w   d2,d2
                adda.w  d2,a0
                bra.b   $C2AD00
