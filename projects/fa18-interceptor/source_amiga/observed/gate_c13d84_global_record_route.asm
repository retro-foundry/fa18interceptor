; Byte-exact observed C13D84 global/record route gate $C14F06-$C14F1B.
; The adjacent setup route is entered only when the global word is zero and
; bit 1 of the global record-status word is clear.

                org     $C14F06

GLOBAL_ROUTE_WORD               equ     $C461F2
GLOBAL_RECORD_STATUS             equ     $C458D2

gate_c13d84_global_record_route:
                move.w  GLOBAL_ROUTE_WORD,d0
                tst.w   d0
                bne.b   $C14F8A
                move.w  GLOBAL_RECORD_STATUS,d0
                btst    #1,d0
                bne.b   $C14F6C
