; Byte-exact observed C13D84 record word-$6E update tail $C1484C-$C14875.
; It runs the indexed record updater, adds local word -$20 to active-record
; word +$78, stores the result at +$6E, and restores the caller frame.

                org     $C1484C

INDEXED_RECORD_UPDATER          equ     $C26428
CURRENT_CONTROL_RECORD          equ     $C18210

finish_c13d84_record_word6e_update:
                jsr     INDEXED_RECORD_UPDATER.l
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  $78(a0),d0
                movea.l -$20(a6),a0
                move.w  (a0),d1
                add.w   d1,d0
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  d0,$6E(a0)
                movem.l (a7)+,d2/a2-a5
                unlk    a6
                rts
