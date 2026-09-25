; Byte-exact $C1EEA0-$C1EEA5 shared zero-return after a stream sentinel.
; Run037's C37EA6 control stream reaches this exit and returns to C1CC88.
                org $C1EEA0
return_zero_from_c1ee14_stream_scan:
 moveq #0,d0
 unlk a6
 rts
