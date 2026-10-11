; Byte-exact observed mode-zero record-motion continuation $C23FC2-$C23FCF.
; It adds $1E0 to record word $6C, retaining the incremented value only when
; it does not exceed the observed $4200 bound.

                org     $C23FC2

clamp_mode_zero_record_angle_increment:
                move.w  $6c(a1),d0
                addi.w  #$1e0,d0
                cmpi.w  #$4200,d0
                ble.b   $C23FD4
