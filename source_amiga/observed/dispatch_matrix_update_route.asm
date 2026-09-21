; Byte-exact reconstruction of $C2D99C-$C2D9AF (Hunk 32 +$594).
; Dispatches the observed matrix update between two local route bodies.

                org     $C2D99C

MATRIX_ROUTE_STATE          equ $C45785
run_enabled_matrix_route    equ $C2D9BA
run_disabled_matrix_route   equ $C2DB18

dispatch_matrix_update_route:
                tst.b   MATRIX_ROUTE_STATE.l
                beq.b   .disabled_route
                bsr.w   run_enabled_matrix_route
                rts
.disabled_route:
                bsr.w   run_disabled_matrix_route
                rts
