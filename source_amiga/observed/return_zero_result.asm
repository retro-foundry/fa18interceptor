; Byte-exact $C1FE20 zero-result leaf observed in attract frame 1800.
; Caller and higher-level result meaning are unassigned.
                org $C1FE20
return_zero_result:
 moveq #0,d0
 rts
