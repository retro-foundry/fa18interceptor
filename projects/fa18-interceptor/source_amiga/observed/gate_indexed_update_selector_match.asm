; Byte-exact observed indexed-update selector match gate $C25B94-$C25BA5.
; It masks the shared selector to five bits and compares it with the selected
; record index; mismatch takes the normal record flag handling continuation.

                org     $C25B94

INDEXED_UPDATE_SELECTOR         equ     $C458DA
SELECTED_RECORD_INDEX            equ     $C459B4

gate_indexed_update_selector_match:
                move.w  INDEXED_UPDATE_SELECTOR,d0
                andi.w  #$1F,d0
                cmp.w   SELECTED_RECORD_INDEX,d0
                bne.b   $C25BAC
