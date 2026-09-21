; Byte-exact static-only variant-pair publication $C33B80-$C33B8B.
                org     $C33B80
POSTFLIGHT_VARIANT_PAIR         equ $C4593E
POSTFLIGHT_VARIANT_CONTINUATION equ $C33C18
publish_postflight_variant_pair:
                movem.w d0-d1,POSTFLIGHT_VARIANT_PAIR.l
                bra.w   POSTFLIGHT_VARIANT_CONTINUATION
