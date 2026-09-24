; Byte-exact C340DC-C34145 mirrored postflight renderer sweep and return stub.
                org     $C340DC
POSTFLIGHT_X_BASE               equ     $C4598C
POSTFLIGHT_X_OFFSET             equ     $C45988
SUBMIT_SHARED_RENDERER          equ     $C2F5F4
SUBMIT_ADJUSTED_RENDERER        equ     $C2F5D4

run_postflight_mirrored_sweep:
                move.w  POSTFLIGHT_X_BASE.l,d0
                add.w   POSTFLIGHT_X_OFFSET.l,d0
                move.w  #0,-2(a6)
postflight_mirrored_sweep_loop:
                move.w  -4(a6),d1
                subi.w  #10,d0
                blt.b   postflight_mirrored_sweep_done
                cmpi.w  #$13F,d0
                bgt.b   postflight_mirrored_sweep_done
                move.w  #$85,d2
                add.w   POSTFLIGHT_X_OFFSET.l,d2
                cmp.w   d2,d0
                ble.b   postflight_mirrored_sweep_done
                move.w  d0,-(a7)
                cmpi.w  #1,-2(a6)
                beq.b   postflight_mirrored_sweep_adjusted
                cmpi.w  #3,-2(a6)
                beq.b   postflight_mirrored_sweep_adjusted
                jsr     SUBMIT_SHARED_RENDERER.l
                bra.b   postflight_mirrored_sweep_restore
postflight_mirrored_sweep_adjusted:
                jsr     SUBMIT_ADJUSTED_RENDERER.l
                move.w  -4(a6),d1
                subq.w  #1,d1
                jsr     SUBMIT_SHARED_RENDERER.l
postflight_mirrored_sweep_restore:
                move.w  (a7)+,d0
                addq.w  #1,-2(a6)
                bra.b   postflight_mirrored_sweep_loop
postflight_mirrored_sweep_done:
                unlk    a6
                rts
postflight_mirrored_sweep_return_stub:
                rts
