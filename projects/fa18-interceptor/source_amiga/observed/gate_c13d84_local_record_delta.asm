; Byte-exact observed C13D84 local-record delta gate $C14240-$C14253.
; Bit 7 of the frame-local record word selects the later alternate route.  If
; clear, the signed local -$1C is tested against $1D4C before either fixed
; adjustment continuation.

                org     $C14240

gate_c13d84_local_record_delta:
                movea.l -$30(a6),a0
                move.w  (a0),d0
                btst    #7,d0
                bne.b   $C14268
                cmpi.w  #$1D4C,-$1C(a6)
                ble.b   $C1425E
