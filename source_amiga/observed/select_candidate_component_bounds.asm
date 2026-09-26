; Byte-exact observed candidate-component bound selection $C26F76-$C26F93.
; With both tested bit-0 flags clear, it selects $7000 and $1000 bounds;
; otherwise the adjacent path chooses an alternate bound pair.

                org     $C26F76

select_candidate_component_bounds:
                btst    #0,-$16(a6)
                bne.b   $C26FA2
                btst    #0,$2(a0,d0.w)
                bne.b   $C26FA2
                dc.w    $227C,$0000,$7000       ; movea.l #$7000,a1
                dc.w    $247C,$0000,$1000       ; movea.l #$1000,a2
                bra.b   $C26FAE
