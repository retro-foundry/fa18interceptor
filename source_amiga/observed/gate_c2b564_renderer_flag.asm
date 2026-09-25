; Byte-exact observed flag gate at $C2B564-$C2B56B.
; The nonpositive route returns through the preceding $C2B562 tail.

                org     $C2B564

RENDERER_FLAG_C457AD           equ     $C457AD
RENDERER_FLAG_RETURN            equ     $C2B562

gate_c2b564_renderer_flag:
                tst.b   RENDERER_FLAG_C457AD.l
                ble.b   RENDERER_FLAG_RETURN
