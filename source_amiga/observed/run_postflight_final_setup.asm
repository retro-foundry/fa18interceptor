; Byte-exact static-only final setup $C335A2-$C335B5.
                org     $C335A2
POSTFLIGHT_FINAL_HELPER_A      equ $C33FB4
POSTFLIGHT_FINAL_HELPER_B      equ $C33AD6
run_postflight_final_setup:
                move.w  #$D4,d0
                move.w  #1,d1
                bsr.w   POSTFLIGHT_FINAL_HELPER_A
                move.w  #$CE,d0
                bsr.w   POSTFLIGHT_FINAL_HELPER_B
