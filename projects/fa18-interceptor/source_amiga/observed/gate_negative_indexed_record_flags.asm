; Byte-exact observed negative indexed-record flag gate $C231C4-$C231EF.
; A negative byte at +$38 selects a record-bank entry using its low seven bits;
; three status bits in that entry then decide whether to continue.

                org     $C231C4

INDEXED_RECORD_BANK              equ     $C46184

gate_negative_indexed_record_flags:
                move.b  $38(a2),d0
                bge.b   $C231F0
                andi.w  #$007F,d0
                asl.w   #8,d0
                add.w   d0,d0
                lea     INDEXED_RECORD_BANK.l,a3
                btst    #6,$1(a3,d0.w)
                beq.b   $C2321A
                btst    #1,$20(a3,d0.w)
                bne.b   $C2321A
                btst    #0,$1(a3,d0.w)
                bne.b   $C2321A
