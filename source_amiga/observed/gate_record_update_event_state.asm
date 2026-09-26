; Byte-exact observed record-update event-state gate $C23340-$C23353.
; A negative event byte, or a nonzero stride-state byte, takes the shared
; continuation.  With both tests clear, the observed BEQ transfers to the
; alternate setup at $C233D6.

                org     $C23340

RECORD_UPDATE_EVENT_STATE       equ     $C4579C
RECORD_STRIDE_STATE             equ     $C457AE

gate_record_update_event_state:
                tst.b   RECORD_UPDATE_EVENT_STATE
                blt.b   $C233A8
                tst.b   RECORD_STRIDE_STATE
                bne.b   $C233A8
                beq.w   $C233D6
