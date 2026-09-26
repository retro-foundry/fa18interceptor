; Byte-exact observed record-update event-counter gate $C2325E-$C2326F.
; It clears a companion event byte and routes to the later update stage unless
; the event counter byte equals $7D.

                org     $C2325E

RECORD_EVENT_COMPANION          equ     $C45888
RECORD_EVENT_COUNTER            equ     $C458A6

gate_record_update_event_counter:
                clr.b   RECORD_EVENT_COMPANION
                cmpi.b  #$7D,RECORD_EVENT_COUNTER
                bne.w   $C23340
