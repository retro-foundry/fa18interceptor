; Byte-exact observed record-product output initialization $C25864-$C25875.
; It clears the shared selector word and resets the product output pointer to
; the fixed workspace used by the subsequent product helper.

                org     $C25864

RECORD_PRODUCT_SELECTOR         equ     $C46182
RECORD_PRODUCT_OUTPUT_POINTER   equ     $C459CA
RECORD_PRODUCT_OUTPUT_BASE      equ     $C4E2BC

initialize_record_product_output_pointer:
                clr.w   RECORD_PRODUCT_SELECTOR
                move.l  #RECORD_PRODUCT_OUTPUT_BASE,RECORD_PRODUCT_OUTPUT_POINTER
                rts
