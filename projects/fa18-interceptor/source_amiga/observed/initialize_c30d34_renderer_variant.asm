; Byte-exact observed C30D34 renderer-variant setup $C30D32-$C30D61.
; A completed prior entry returns, then this entry initializes fixed renderer
; parameters, applies the shared long offset, and selects the continuation
; based on the signed result and a status byte.

                org     $C30D32

RENDERER_WORK_OFFSET             equ     $C45918
RENDERER_STATUS_BYTE             equ     $C45846
RENDERER_VARIANT_HELPER          equ     $C310E2

initialize_c30d34_renderer_variant:
                rts
                move.l  #$1C70,d1
                movea.w #2,a4
                move.w  #$202,d6
                movea.w #$25,a5
                move.w  #0,d7
                add.l   RENDERER_WORK_OFFSET.l,d1
                bsr.w   RENDERER_VARIANT_HELPER
                blt.b   $C30D7E
                move.w  #$30A,d2
                tst.b   RENDERER_STATUS_BYTE.l
                ble.b   $C30D70
