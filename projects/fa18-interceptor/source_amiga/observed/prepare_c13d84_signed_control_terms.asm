; Byte-exact observed C13D84 continuation $C1414E-$C1419D.
; It prepares signed local control terms, optionally calls the C148E2 helper,
; then applies two observed state-bit gates.  Field semantics remain open.

                org     $C1414E

CONTROL_GATE_WORD_A             equ     $C458CC
CONTROL_GATE_WORD_B             equ     $C458CE
SIGNED_TERM_HELPER              equ     $C148E2

prepare_c13d84_signed_control_terms:
                movea.l -$4(a6),a0
                move.b  (a0),d0
                ext.w   d0
                ext.l   d0
                asl.l   #8,d0
                move.w  -$18(a6),d1
                neg.w   d1
                move.w  d0,-$24(a6)
                move.w  d1,-$1c(a6)
                cmpi.w  #$800,d1
                bge.b   $C14174
                clr.w   -$1a(a6)
                bra.b   $C1417C
                bsr.w   SIGNED_TERM_HELPER
                move.w  d0,-$1a(a6)
                move.w  CONTROL_GATE_WORD_A.l,d0
                btst    #6,d0
                beq.w   $C146C2
                move.w  CONTROL_GATE_WORD_B.l,d0
                btst    #14,d0
                bne.w   $C146C2
                tst.w   -$24(a6)
                bne.b   $C141AC
