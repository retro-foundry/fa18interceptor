; Byte-exact observed C13D84 continuation $C147D0-$C14831.
; It selects a local signed range, then uses the bounded C148A2 stack helper
; when the byte addressed by local -$4 lies strictly inside that range.

                org     $C147D0

SIGNED_RANGE_HELPER             equ     $C148A2

gate_c13d84_signed_local_range:
                movea.l -$10(a6),a0
                move.b  (a0),d0
                btst    #3,d0
                beq.b   $C147E4
                move.w  #$6C,-$16(a6)
                bra.b   $C147F0
                move.w  -$18(a6),d0
                neg.w   d0
                movea.l -$20(a6),a0
                move.w  d0,(a0)
                tst.w   -$16(a6)
                beq.b   $C1484C
                movea.l -$10(a6),a0
                move.b  (a0),d0
                btst    #3,d0
                beq.b   $C14840
                movea.l -$4(a6),a0
                move.b  (a0),d0
                ext.w   d0
                ext.l   d0
                move.w  -$16(a6),d1
                ext.l   d1
                cmp.l   d1,d0
                bge.b   $C14832
                neg.l   d1
                cmp.l   d1,d0
                ble.b   $C14832
                moveq   #3,d0
                move.l  d0,-(sp)
                moveq   #7,d0
                move.l  d0,-(sp)
                move.l  -$20(a6),-(sp)
                bsr.w   SIGNED_RANGE_HELPER
                lea     $c(sp),sp
                bra.b   $C1484C
