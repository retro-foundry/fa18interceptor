; Byte-exact alternate helper entry $C1D0A4-$C1D0B5.
; Its D7 selector-to-index dataflow is static evidence; record ownership is unknown.
                org $C1D0A4
ALTERNATE_RECORD_TABLE_BASE     equ $C48184

prepare_alternate_record_table_index:
                lea ALTERNATE_RECORD_TABLE_BASE.l,a2
                move.w d7,d1
                move.w d7,d6
                andi.w #$ff00,d1
                asr.w #3,d1
                bra.b $C1D0C6
