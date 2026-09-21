; Byte-exact indexed command-state clear wrapper $C17B08-$C17B2B.

                org     $C17B08

INDEXED_COMMAND_STATE_TABLE     equ $C4FE38
CALL_C4FFB0                     equ $C4FFB0

clear_indexed_command_state:
                link.w  a6,#0
                move.l  8(a6),d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #INDEXED_COMMAND_STATE_TABLE,a0
                clr.l   (a0)
                move.l  8(a6),-(a7)
                jsr     CALL_C4FFB0.l
                addq.l  #4,a7
                unlk    a6
                rts
