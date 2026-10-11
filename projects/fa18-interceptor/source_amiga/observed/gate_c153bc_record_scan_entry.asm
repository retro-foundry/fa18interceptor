; Byte-exact observed basic block $C153BC-$C153C7.

                org     $C153BC

gate_c153bc_record_scan_entry:
                movea.l -$14(a6),a0
                move.w  (a0),d0
                btst    #0,d0
                beq.b   $C153D4
