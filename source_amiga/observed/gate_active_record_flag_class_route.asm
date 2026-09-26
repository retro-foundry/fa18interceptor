; Byte-exact observed active-record flag/class route $C14C1E-$C14C49.
; Two high source-record bits gate this route; otherwise active-record byte
; +$7C and exact class byte +$62 choose its continuation.

                org     $C14C1E

CURRENT_CONTROL_RECORD          equ     $C18210

gate_active_record_flag_class_route:
                movea.l -$30(a6),a0
                move.b  (a0),d0
                andi.b  #$C0,d0
                tst.b   d0
                beq.w   $C14CCA
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  $7C(a0),d0
                andi.b  #$70,d0
                tst.b   d0
                bne.b   $C14CA0
                move.b  $62(a0),d0
                cmpi.b  #$14,d0
                bne.b   $C14C72
