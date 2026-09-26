; Byte-exact observed indexed-update continuation $C25C2E-$C25C3D.
; It retains D1's original word in D0, tests command bit $1000 in D1, and
; otherwise routes according to command bit $0100 from the saved word.

                org     $C25C2E

gate_indexed_update_command_bits:
                move.w  d1,d0
                andi.w  #$1000,d1
                bne.b   $C25C3E
                andi.w  #$100,d0
                beq.w   $C25DD0
