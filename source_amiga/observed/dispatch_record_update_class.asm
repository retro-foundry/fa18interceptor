; Byte-exact record-update class dispatch $C23A7E-$C23AA3.
; The observed no-key path reaches the generic nonzero, non-$30$ route.

                org     $C23A7E

PENDING_RECORD_EVENT             equ $C457AE
INDEXED_RECORD_CLASS             equ $62

dispatch_record_update_class:
                tst.b   PENDING_RECORD_EVENT.l
                bne.b   $C23A7A
                move.b  INDEXED_RECORD_CLASS(a1),d0
                andi.b  #$F0,d0
                dc.w    $0C00,$0000             ; cmpi.b #0,d0
                beq.w   $C23F4A
                cmpi.b  #$30,d0
                beq.w   $C23D82
                cmpi.b  #$20,d0
                dc.w    $67D6                   ; beq.b $C23A7A
