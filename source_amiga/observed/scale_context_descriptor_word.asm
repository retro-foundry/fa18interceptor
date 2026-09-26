; Byte-exact observed context-descriptor word path $C1E39A-$C1E3AB.
; A zero word at descriptor +$0E takes the direct descriptor continuation.
; Otherwise it is shifted by the descriptor low-nibble count and forwarded to
; the shared copy/rotate stage.

                org     $C1E39A

scale_context_descriptor_word:
                move.w  $E(a1),d3
                beq.b   $C1E3BC
                dc.w    $E563              ; asl.w d2,d3; retain original encoding
                dc.w    $D2FC,$000A         ; adda.w #$A,a1; retain original encoding
                move.w  d3,d1
                bra.w   $C1E436
