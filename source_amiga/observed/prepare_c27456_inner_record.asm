; Byte-exact indexed-triple plane-side scan prefix $C27456-$C27477.
; It operates in the caller's shared frame and walks the A4 candidate stream.

                org     $C27456

INNER_RECORD_NEGATIVE_PATH      equ     $C27450

prepare_c27456_inner_record:
                move.l  (a4)+,d2
                blt.b   INNER_RECORD_NEGATIVE_PATH
                movea.l d2,a5
                ; MOVEM.W $2(A5),D2-D4; retain observed register-mask words.
                dc.w    $4cad,$001c,$0002
                andi.w  #$fff,d4
                addi.w  #$a4,d2
                addi.w  #$a4,d3
                addi.w  #$a4,d4
                ; LEA $0(A3,D2.W),A5; retain observed indexed EA words.
                dc.w    $4bf3,$2000
                move.w  d4,d0
