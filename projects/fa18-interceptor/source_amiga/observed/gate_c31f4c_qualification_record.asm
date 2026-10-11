; Byte-exact observed qualification-record gate $C31F4C-$C31F67.
; The unobserved $C31F68 branch body and later mode gate are not claimed.

                org     $C31F4C

QUALIFICATION_RECORD_BASE        equ     $C46184
QUALIFICATION_RECORD_OFFSET      equ     $C458DE
QUALIFICATION_RECORD_SKIP        equ     $C31F6A

gate_c31f4c_qualification_record:
                lea     QUALIFICATION_RECORD_BASE.l,a1
                adda.w  QUALIFICATION_RECORD_OFFSET.l,a1
                moveq   #0,d0
                ; Keep the captured zero-displacement form, not (A1) direct.
                dc.w    $0829,$0007,$0000
                bne.b   QUALIFICATION_RECORD_SKIP
                move.w  $6E(a1),d0
                bge.b   QUALIFICATION_RECORD_SKIP
