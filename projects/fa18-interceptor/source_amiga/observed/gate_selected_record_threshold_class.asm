; Byte-exact observed selected-record threshold-class gate $C14CE6-$C14D01.
; It routes on bits 4-6 of byte +$7C, then tests whether byte +$62 equals
; $14 before the adjacent threshold assignments.

                org     $C14CE6

CURRENT_CONTROL_RECORD          equ     $C18210

gate_selected_record_threshold_class:
                dc.w    $2079                   ; movea.l $C18210,a0
                dc.l    CURRENT_CONTROL_RECORD
                move.b  $7C(a0),d0
                andi.b  #$70,d0
                tst.b   d0
                bne.b   $C14D16
                move.b  $62(a0),d0
                cmpi.b  #$14,d0
                bne.b   $C14D0C
