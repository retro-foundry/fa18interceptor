; Byte-exact zero-index update route $C25B80-$C25B93.

                org     $C25B80

SELECTED_RECORD_INDEX            equ $C459B4
INDEXED_UPDATE_CONTEXT           equ $C45785

route_zero_indexed_update:
                tst.w   SELECTED_RECORD_INDEX.l
                bne.b   $C25B94
                tst.b   INDEXED_UPDATE_CONTEXT.l
                bne.b   $C25BAC
                bra.w   $C25C3E
