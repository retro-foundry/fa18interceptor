; Byte-exact observed qualification-mode gate $C31F6A-$C31F71.
; The nonzero body is unobserved; zero mode enters the candidate-line path.

                org     $C31F6A

QUALIFICATION_MODE_FLAG          equ     $C45785
QUALIFICATION_MODE_ZERO          equ     $C31F86

gate_c31f6a_qualification_mode:
                tst.b   QUALIFICATION_MODE_FLAG.l
                beq.b   QUALIFICATION_MODE_ZERO
