; Byte-exact observed scaled-control update gate $C130AE-$C130C3.
; It takes the standard invocation route unless both the local record's bit 3
; and the global control byte's bit 0 select the alternate invocation.

                org     $C130AE

GLOBAL_CONTROL_BIT_SOURCE       equ     $C45B5B

gate_scaled_control_update_route:
                movea.l -$10(a6),a0
                move.w  (a0),d0
                btst    #3,d0
                beq.b   $C130C4
                btst    #0,GLOBAL_CONTROL_BIT_SOURCE
                bne.b   $C130E4
