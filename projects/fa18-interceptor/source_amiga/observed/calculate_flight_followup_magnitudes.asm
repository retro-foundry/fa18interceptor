; Byte-exact magnitude preparation phase $C1CD0E-$C1CDB1.
; Inputs are the indexed record selected by the C1CCBC setup prefix.

                org     $C1CD0E

SELECTOR_X_WORD                 equ $C4594C
SELECTOR_Z_WORD                 equ $C4594E
PREPARED_X_WORD                 equ $C45A72
PREPARED_Z_WORD                 equ $C45A76
PREPARED_DEPTH_LONG             equ $C45A78
MAGNITUDE_ERROR_WORD            equ $C4599E
MAGNITUDE_ERROR_HELPER          equ $C06C02
MAGNITUDE_MAXIMUM               equ $00EF

calculate_flight_followup_magnitudes:
                moveq   #0,d7
                move.w  $06(a0),d7
                sub.w   SELECTOR_X_WORD.l,d7
                swap    d7
                asl.l   #6,d7
                move.l  $14(a0),d4
                andi.l  #$3fffff,d4
                add.l   d4,d7
                moveq   #0,d6
                move.w  $08(a0),d6
                sub.w   SELECTOR_Z_WORD.l,d6
                swap    d6
                asl.l   #6,d6
                move.l  $1c(a0),d4
                andi.l  #$3fffff,d4
                add.l   d4,d6
                move.l  d7,d2
                move.l  d6,d4
                asr.l   #8,d7
                asr.l   #8,d6
                move.w  PREPARED_X_WORD.l,d0
                ext.l   d0
                add.l   d0,d7
                bge.b   calculate_flight_followup_x_nonnegative
                neg.l   d7
calculate_flight_followup_x_nonnegative:
                asr.l   #8,d7
                asr.l   #4,d7
                move.w  PREPARED_Z_WORD.l,d0
                ext.l   d0
                add.l   d0,d6
                bge.b   calculate_flight_followup_z_nonnegative
                neg.l   d6
calculate_flight_followup_z_nonnegative:
                asr.l   #8,d6
                asr.l   #4,d6
                move.l  $10(a0),d0
                add.l   PREPARED_DEPTH_LONG.l,d0
                bge.b   calculate_flight_followup_depth_nonnegative
                neg.l   d0
calculate_flight_followup_depth_nonnegative:
                asr.l   #8,d0
                asr.l   #3,d0
                cmp.w   d6,d7
                bge.b   calculate_flight_followup_x_maximum
                move.w  d6,d7
                cmp.w   d0,d7
                bge.b   calculate_flight_followup_max_ready
                move.w  d0,d7
                bra.b   calculate_flight_followup_max_ready
calculate_flight_followup_x_maximum:
                cmp.w   d0,d7
                bge.b   calculate_flight_followup_max_ready
                move.w  d0,d7
calculate_flight_followup_max_ready:
                lsr.w   #1,d7
                cmpi.w  #MAGNITUDE_MAXIMUM,d7
                ble.b   calculate_flight_followup_done
                move.w  #$29,MAGNITUDE_ERROR_WORD.l
                jsr     MAGNITUDE_ERROR_HELPER.l
                move.w  #MAGNITUDE_MAXIMUM,d7
calculate_flight_followup_done:
                ; Falls through into the C1CDB2 lookup-table shift phase.
