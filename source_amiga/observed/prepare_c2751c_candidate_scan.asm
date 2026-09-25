; Byte-exact observed continuation $C2751C-$C2755B of the shared candidate
; scan frame.  The preceding no-candidate exit is reconstructed separately.

                org     $C2751C

CANDIDATE_SCAN_LIMIT            equ     $C4585E
CANDIDATE_SCAN_POINTER          equ     $C459C6
CANDIDATE_SCAN_REJECT           equ     $C275FC

prepare_c2751c_candidate_scan:
                move.b  CANDIDATE_SCAN_LIMIT.l,d0
                ext.w   d0
                move.w  d0,-$6(a6)
                clr.l   -$a(a6)
                moveq   #-1,d5
                movea.l CANDIDATE_SCAN_POINTER.l,a5
.next_candidate:
                move.w  (a5),-$2(a6)
                ; SUBA.W #$0018,A5; preserve the original immediate encoding.
                dc.w    $9afc,$0018
                addq.w  #1,-$8(a6)
                btst    #4,-$1(a6)
                bne.w   CANDIDATE_SCAN_REJECT
                btst    #6,-$1(a6)
                bne.w   CANDIDATE_SCAN_REJECT
                cmpi.w  #$a,d5
                bge.w   CANDIDATE_SCAN_REJECT
