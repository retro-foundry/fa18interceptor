; Byte-exact observed context record-copy iteration $C1E376-$C1E38D.
; It decrements the bounded count, reads a descriptor word, publishes its low
; nibble, then routes on descriptor bit 6.

                org     $C1E376

CONTEXT_DESCRIPTOR_LOW_NIBBLE   equ     $C45AB8

begin_context_record_copy_iteration:
                move.w  d7,d0
                subq.w  #1,d0
                move.w  (a1)+,d1
                move.w  d1,d2
                andi.w  #$F,d2
                move.w  d2,CONTEXT_DESCRIPTOR_LOW_NIBBLE
                btst    #6,d1
                beq.b   $C1E39A
