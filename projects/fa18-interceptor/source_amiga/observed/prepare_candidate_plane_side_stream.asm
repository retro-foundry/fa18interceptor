; Byte-exact observed candidate plane-side stream preparation $C2739A-$C273CD.
; It clears record bit 14, selects one of two record-byte flags from a local
; condition, stores a local word, then chooses the alternate static stream for
; class $20 before the common continuation.

                org     $C2739A

CANDIDATE_ALTERNATE_STREAM       equ     $C391E4

prepare_candidate_plane_side_stream:
                andi.w  #$BFFF,$2(a4)
                tst.w   -$44(a6)
                bne.b   $C273AE
                ori.b   #$40,$4(a4)
                bra.b   $C273B4
                ori.b   #$80,$4(a4)
                move.w  -$34(a6),$4E(a4)
                move.l  $10(a4),d3
                cmpi.b  #$20,$62(a3)
                bne.b   $C273CE
                lea     CANDIDATE_ALTERNATE_STREAM.l,a4
                bra.b   $C273D4
