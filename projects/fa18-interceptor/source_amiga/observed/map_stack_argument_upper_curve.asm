; Byte-exact observed stack-argument upper curve $C131DA-$C131F3.
; Values above eight are subtracted from $78, arithmetically quartered, then
; subtracted from $3F and written back to the stack argument slot.

                org     $C131DA

map_stack_argument_upper_curve:
                move.w  $A(a6),d0
                cmpi.w  #8,d0
                ble.b   $C131F4
                moveq   #$78,d1
                sub.w   d0,d1
                asr.w   #2,d1
                moveq   #$3F,d0
                sub.w   d1,d0
                move.w  d0,$A(a6)
                bra.b   $C13204
