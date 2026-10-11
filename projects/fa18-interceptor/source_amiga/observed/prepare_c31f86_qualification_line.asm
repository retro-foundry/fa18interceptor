; Byte-exact observed qualification candidate-line path $C31F86-$C31FBB.
; The mode-specific path selected by the final BEQ is outside this slice.

                org     $C31F86

QUALIFICATION_CANDIDATE_TABLE    equ     $C458F8
QUALIFICATION_CANDIDATE_HELPER   equ     $C31C20
QUALIFICATION_QUOTIENT           equ     $C45B1E
QUALIFICATION_POST_DIVIDE        equ     $C25A08
QUALIFICATION_MODE_FLAG          equ     $C45785
QUALIFICATION_LINE_BASE          equ     $C457FF
QUALIFICATION_MODE_ZERO          equ     $C31FCA

prepare_c31f86_qualification_line:
                lea     QUALIFICATION_CANDIDATE_TABLE.l,a1
                bsr.w   QUALIFICATION_CANDIDATE_HELPER
                bge.b   .candidate_ready
                rts
.candidate_ready:
                ext.l   d0
                divu.w  #$C,d0
                ext.l   d0
                move.l  d0,QUALIFICATION_QUOTIENT.l
                jsr     QUALIFICATION_POST_DIVIDE.l
                moveq   #3,d0
                lea     QUALIFICATION_LINE_BASE.l,a2
                lea     4(a2),a0
                tst.b   QUALIFICATION_MODE_FLAG.l
                beq.b   QUALIFICATION_MODE_ZERO
