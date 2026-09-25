; Byte-exact observed second component-difference setup $C20862-$C20889.

                org     $C20862

COMPONENT_REFERENCE_X           equ     $C45B2A
COMPONENT_REFERENCE_Z           equ     $C45B2E
COMPONENT_DIFFERENCE_HELPER     equ     $C2574A

prepare_c20862_second_component_difference:
                ; MOVEM.W D5-D7,-(SP); retain observed register-mask words.
                dc.w    $48a7,$0700
                move.w  -$26(a6),d5
                sub.w   COMPONENT_REFERENCE_X.l,d5
                clr.w   d6
                move.w  -$22(a6),d7
                sub.w   COMPONENT_REFERENCE_Z.l,d7
                move.w  #$100,d0
                jsr     COMPONENT_DIFFERENCE_HELPER.l
                ; MOVEM.W (SP)+,D0-D2; retain observed register-mask words.
                dc.w    $4c9f,$0007
