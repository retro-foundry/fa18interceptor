; Byte-exact observed coordinate-update threshold gate $C12388-$C123BB.
; It clears the flight-update mode byte and flag bit 1, resets a local byte,
; then classifies two signed local words against $0384 and $0A8C thresholds.

                org     $C12388

FLIGHT_UPDATE_MODE_BYTE         equ     $C4586B
SEQUENCE_FLAGS                  equ     $C458CA

gate_coordinate_update_thresholds:
                clr.b   FLIGHT_UPDATE_MODE_BYTE.l
                move.w  SEQUENCE_FLAGS.l,d0
                andi.w  #$FFFD,d0
                move.w  d0,SEQUENCE_FLAGS.l
                clr.b   -$5(a6)
                move.w  -$2(a6),d0
                cmpi.w  #$384,d0
                blt.b   $C123B2
                cmpi.w  #$A8C,d0
                blt.b   $C123CA
                move.w  -$4(a6),d0
                cmpi.w  #$384,d0
                blt.b   $C123E0
