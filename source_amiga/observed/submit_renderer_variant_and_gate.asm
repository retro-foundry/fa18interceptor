; Byte-exact observed renderer-variant submission and follow-up gate
; $C30D70-$C30D85.  It prepares fixed blit parameters, invokes the renderer
; helper, then tests the activity byte before the existing continuation.

                org     $C30D70

RENDERER_VARIANT_SUBMIT         equ     $C30CC4
RENDERER_ACTIVITY_BYTE          equ     $C45845

submit_renderer_variant_and_gate:
                moveq   #$C,d4
                move.w  #$3F,d0
                move.w  #$FFFE,d3
                bsr.w   RENDERER_VARIANT_SUBMIT
                tst.b   RENDERER_ACTIVITY_BYTE
                ble.b   $C30D32
