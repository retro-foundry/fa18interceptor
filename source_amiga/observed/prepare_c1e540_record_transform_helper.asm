; Byte-exact observed C1E540 transform continuation $C1E67C-$C1E689.
; It preserves D2-D4 in the transform working registers, loads the current
; record word, and tests its bit 6 before selecting a helper sequence.

                org     $C1E67C

prepare_c1e540_record_transform_helper:
                move.l  d2,d5
                move.l  d3,d6
                move.l  d4,d7
                move.w  (a1),d1
                dc.w    $0801,$0006             ; btst.b #6,d1
                beq.b   $C1E694
