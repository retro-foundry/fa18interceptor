; Byte-exact paired-record rejection prefilter $C231A2-$C231C3.
; The observed rejection continuation at $C2321A returns D0=0/Z=1;
; success/alternate continuations may not share that return contract.

                org     $C231A2

C2321A                         equ     $C2321A
C2321E                         equ     $C2321E
C457BC                         equ     $C457BC

gate_c231a2_record_flags:
                ; Preserve the observed zero-displacement address encoding.
                dc.w    $302a,$0000
                andi.w  #$8700,d0
                bne.b   C2321A
                btst    #1,$20(a2)
                bne.b   C2321A
                tst.b   C457BC.l
                bne.b   C2321E
                btst    #6,1(a2)
                beq.b   C2321A
