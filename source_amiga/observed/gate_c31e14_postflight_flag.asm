; Byte-exact observed scaling and flag gate $C31E14-$C31E21.

                org     $C31E14

POSTFLIGHT_FLAGS                equ     $C458CA

gate_c31e14_postflight_flag:
                asr.w   #3,d0
                move.w  POSTFLIGHT_FLAGS.l,d1
                andi.w  #2,d1
                beq.b   $C31E28
