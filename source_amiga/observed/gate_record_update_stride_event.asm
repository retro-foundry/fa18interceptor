; Byte-exact observed record-update stride/event gate $C233D6-$C233E9.
; A nonzero stride-state byte exits to the shared continuation.  Otherwise the
; low bit at record +$02 must be set before the following event-class route.

                org     $C233D6

RECORD_STRIDE_STATE             equ     $C457AE

gate_record_update_stride_event:
                tst.b   RECORD_STRIDE_STATE
                bne.w   $C2354A
                btst    #0,$2(a1)
                beq.w   $C2354A
