; Byte-exact observed C13D84 active-control-record class gate $C1434A-$C1435B.
; Bits 4-6 of active-record byte +$7C select the shared continuation.  With
; those bits clear, the following path scales the signed local -$16.

                org     $C1434A

CURRENT_CONTROL_RECORD          equ     $C18210

gate_c13d84_control_record_class:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  $7C(a0),d0
                andi.b  #$70,d0
                tst.b   d0
                bne.b   $C14366
