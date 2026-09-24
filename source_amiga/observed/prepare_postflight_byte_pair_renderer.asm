; Byte-exact C345A0-C345EF frame setup and bounds gate for byte-pair renderer.
                org     $C345A0
RENDERER_X_OFFSET                equ     $C45988
POSTFLIGHT_BYTE_PAIR_FALLBACK    equ     $C34696

prepare_postflight_byte_pair_renderer:
                link.w  a6,#-16
                move.w  d0,-2(a6)
                move.w  d1,-4(a6)
                move.w  d2,-14(a6)
                cmpi.w  #14,d0
                ble.w   POSTFLIGHT_BYTE_PAIR_FALLBACK
                cmpi.w  #$132,d0
                bge.w   POSTFLIGHT_BYTE_PAIR_FALLBACK
                move.w  #$63,d2
                add.w   RENDERER_X_OFFSET.l,d2
                cmp.w   d2,d0
                ble.w   POSTFLIGHT_BYTE_PAIR_FALLBACK
                move.w  #$DB,d2
                add.w   RENDERER_X_OFFSET.l,d2
                cmp.w   d2,d0
                bge.w   POSTFLIGHT_BYTE_PAIR_FALLBACK
                cmpi.w  #$39,d1
                ble.w   POSTFLIGHT_BYTE_PAIR_FALLBACK
                cmpi.w  #$84,d1
                bge.w   POSTFLIGHT_BYTE_PAIR_FALLBACK
