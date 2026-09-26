; Byte-exact observed record-update continuation $C23D72-$C23D7D.
; It masks D1 to event bit $0002; without that bit it returns the observed
; success result one, otherwise it takes the pending-event route.

                org     $C23D72

return_record_update_without_event_bit:
                andi.w  #$2,d1
                bne.w   $C23FF8
                moveq   #1,d0
                rts
