; Byte-exact observed indexed-update auxiliary-byte gate $C25FC2-$C25FD3.
; A nonzero secondary index bypasses this update.  Otherwise the signed shared
; byte is loaded; zero enters the later candidate-scan preparation while its
; sign selects the following adjustment path.

                org     $C25FC2

SECONDARY_RECORD_INDEX           equ     $C459B6
INDEXED_UPDATE_AUXILIARY_BYTE    equ     $C45847

gate_indexed_update_auxiliary_byte:
                tst.w   SECONDARY_RECORD_INDEX
                bne.b   $C25FEE
                move.b  INDEXED_UPDATE_AUXILIARY_BYTE,d1
                blt.b   $C25FD8
                ble.b   $C25FEE
