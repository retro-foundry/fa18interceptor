; Byte-exact observed C13D84 local flagged-value gate $C14744-$C1475B.
; The adjacent special path requires bit 7 in one local word, bit 11 in a
; second local word, and a third local word greater than $40.

                org     $C14744

gate_c13d84_local_flagged_value:
                movea.l -$30(a6),a0
                move.w  (a0),d0
                btst    #7,d0
                beq.b   $C1477C
                movea.l -$2C(a6),a0
                move.w  (a0),d0
                btst    #11,d0
                beq.b   $C1477C
