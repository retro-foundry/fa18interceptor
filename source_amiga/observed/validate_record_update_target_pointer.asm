; Byte-exact record-update target pointer validation $C24064-$C2407D.

                org     $C24064

RECORD_ARRAY_BASE                equ $C46184

validate_record_update_target_pointer:
                dc.w    $082B,$0006,$0001       ; btst.b #6,$01(a3)
                beq.w   $C242DE
                dc.w    $0069,$0001,$0000       ; ori.w #1,$00(a1)
                move.l  #RECORD_ARRAY_BASE,d0
                cmp.l   a3,d0
                beq.b   $C240E2
