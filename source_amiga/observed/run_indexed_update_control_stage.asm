; Byte-exact control-stage call and bound route $C25C70-$C25C85.

                org     $C25C70

SELECTED_RECORD_INDEX            equ $C459B4
RUN_CONTROL_RECORD_STAGE         equ $C1B27E

run_indexed_update_control_stage:
                jsr     RUN_CONTROL_RECORD_STAGE.l
                tst.w   SELECTED_RECORD_INDEX.l
                bne.w   $C25D22
                move.l  $42(a1),d0
                dc.w    $6C44                   ; bge.b $C25CCA
