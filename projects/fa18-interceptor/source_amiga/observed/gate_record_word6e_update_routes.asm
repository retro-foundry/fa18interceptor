; Byte-exact observed active-record word-$6E update routes $C1503C-$C1506B.
; The active record's word +$6E selects a common route at $03C0; an alternate
; local-record path clears bit 12, while the following path gates on bit 7.

                org     $C1503C

CURRENT_CONTROL_RECORD          equ     $C18210

gate_record_word6e_update_routes:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  $6E(a0),d0
                cmpi.w  #$3C0,d0
                bge.w   $C15130
                movea.l -$2C(a6),a0
                move.w  (a0),d0
                andi.w  #$EFFF,d0
                move.w  d0,(a0)
                bra.w   $C15130
                movea.l -$2C(a6),a0
                move.w  (a0),d0
                btst    #7,d0
                beq.w   $C15112
