; Byte-exact observed record-update component-bound setup $C2407E-$C240AF.
; Non-base records of nonzero class load two three-longword tuples.  A shared
; byte selects one of three absolute-component bounds for the continuation.

                org     $C2407E

RECORD_UPDATE_LEVEL             equ     $C458A7

prepare_record_update_component_bound:
                cmpi.b  #8,$5(a1)
                beq.b   $C240E2
                movem.l $14(a1),d0-d2
                movem.l $C46198.l,d3-d5
                cmpi.b  #3,RECORD_UPDATE_LEVEL.l
                bge.b   $C240B8
                cmpi.b  #2,RECORD_UPDATE_LEVEL.l
                bge.b   $C240B0
                ; Preserve the binary's full immediate MOVE.L encoding.
                dc.w    $2C3C,$0018,$0000       ; move.l #$180000,d6
                bra.b   $C240BE
