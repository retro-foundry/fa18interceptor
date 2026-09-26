; Byte-exact observed postflight callback continuation $C10ECE-$C10ED9.
; It reads the signed callback code from local -$1 and routes values above
; $FF to the common postflight exit path.

                org     $C10ECE

gate_postflight_callback_code_upper_bound:
                move.b  -$1(a6),d0
                cmpi.b  #$ff,d0
                bgt.w   $C10FC8
