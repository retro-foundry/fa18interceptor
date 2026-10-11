; Byte-exact observed scaled control-update invocation $C130E4-$C13111.
; It forms a signed longword input and a 3/8-scaled companion from frame-local
; words, supplies a fixed $20 third argument, then calls the shared wrapper.

                org     $C130E4

SHARED_CONTROL_UPDATE_WRAPPER   equ     $C17D6E

invoke_scaled_control_update:
                move.w  -$A(a6),d0
                ext.l   d0
                move.w  -$4(a6),d1
                ext.l   d1
                move.l  d1,$8(a7)
                asr.l   #2,d1
                move.l  $8(a7),d2
                asr.l   #3,d2
                add.l   d2,d1
                moveq   #$20,d2
                move.l  d2,-(a7)
                move.l  d1,-(a7)
                move.l  d0,-(a7)
                jsr     SHARED_CONTROL_UPDATE_WRAPPER.l
                lea     $C(a7),a7
                bra.b   $C13134
