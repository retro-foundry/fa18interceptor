; Byte-exact observed C13D84 continuation $C13E24-$C13E89.
; It derives a bounded low-nibble count from the byte addressed by local -$C,
; updates that byte when within range, then gates the following record path.

                org     $C13E24

normalize_c13d84_record_nibble:
                movea.l -$8(a6),a0
                move.b  (a0),d0
                andi.b  #3,d0
                tst.b   d0
                beq.w   $C1405C
                movea.l -$c(a6),a0
                move.b  (a0),d0
                ext.w   d0
                ext.l   d0
                andi.l  #$F,d0
                addq.w  #1,d0
                move.w  d0,-$16(a6)
                cmpi.w  #8,d0
                ble.b   $C13E58
                move.w  #8,-$16(a6)
                bra.b   $C13E72
                movea.l -$c(a6),a0
                move.b  (a0),d0
                ext.w   d0
                ext.l   d0
                andi.l  #$F0,d0
                move.w  -$16(a6),d1
                ext.l   d1
                or.l    d1,d0
                move.b  d0,(a0)
                move.w  -$16(a6),d0
                asr.w   #1,d0
                move.w  d0,-$16(a6)
                tst.w   d0
                beq.b   $C13EA0
                movea.l -$4(a6),a0
                move.b  (a0),d0
                tst.b   d0
                bne.b   $C13EA0
