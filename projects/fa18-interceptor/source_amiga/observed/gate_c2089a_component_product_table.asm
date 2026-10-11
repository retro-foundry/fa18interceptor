; Byte-exact observed product-table selection gate $C2089A-$C208AB.

                org     $C2089A

COMPONENT_PRODUCT_TABLE         equ     $C208D4
COMPONENT_SCAN_DEPTH            equ     $C45A78

gate_c2089a_component_product_table:
                ; LEA $C208D4.L,A3; preserve the original absolute-long form.
                dc.w    $47f9,$00c2,$08d4
                cmpi.l  #-$80,COMPONENT_SCAN_DEPTH.l
                bgt.b   $C208B2
