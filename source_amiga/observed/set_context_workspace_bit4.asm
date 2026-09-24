; Byte-exact workspace bit-4 initializer $C1CA82-$C1CB13.
; It touches fixed byte-1 lanes in two mutable workspace regions; ownership is
; structural.  The explicit addresses preserve the original unrolled layout.

                org     $C1CA82

CONTEXT_WORKSPACE_A             equ     $C46184
CONTEXT_WORKSPACE_B             equ     $C48184
CONTEXT_WORKSPACE_FLAG           equ     $10

set_context_workspace_bit4:
                lea     CONTEXT_WORKSPACE_A.l,a0
                move.b  #CONTEXT_WORKSPACE_FLAG,d7
                or.b    d7,$001(a0)
                or.b    d7,$201(a0)
                or.b    d7,$401(a0)
                or.b    d7,$601(a0)
                or.b    d7,$801(a0)
                or.b    d7,$A01(a0)
                or.b    d7,$C01(a0)
                or.b    d7,$E01(a0)
                or.b    d7,$1001(a0)
                or.b    d7,$1201(a0)
                or.b    d7,$1401(a0)
                or.b    d7,$1601(a0)
                or.b    d7,$1801(a0)
                or.b    d7,$1A01(a0)
                or.b    d7,$1C01(a0)
                or.b    d7,$1E01(a0)
                lea     CONTEXT_WORKSPACE_B.l,a0
                or.b    d7,$001(a0)
                or.b    d7,$021(a0)
                or.b    d7,$041(a0)
                or.b    d7,$061(a0)
                or.b    d7,$081(a0)
                or.b    d7,$0A1(a0)
                or.b    d7,$0C1(a0)
                or.b    d7,$0E1(a0)
                or.b    d7,$101(a0)
                or.b    d7,$121(a0)
                or.b    d7,$141(a0)
                or.b    d7,$161(a0)
                or.b    d7,$181(a0)
                or.b    d7,$1A1(a0)
                or.b    d7,$1C1(a0)
                or.b    d7,$1E1(a0)
                rts
