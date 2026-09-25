; Byte-exact no-match path of a 20-slot, 64-byte-stride bit-0 scan.
; The matching-record branch destination is outside this observed path.

                org     $C265E8

C26606                         equ     $C26606
C45C72                         equ     $C45C72

scan_c265e8_flagged_records:
                moveq   #$13,d0
.loop:
                lea     C45C72.l,a0
                move.w  d0,d1
                asl.w   #6,d1
                adda.w  d1,a0
                btst    #0,$27(a0)
                bne.b   C26606
                dbf     d0,.loop
                moveq   #0,d0
                rts
