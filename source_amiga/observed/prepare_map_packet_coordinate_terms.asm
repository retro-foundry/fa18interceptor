; Byte-exact observed M-map packet-coordinate setup $C2AB8C-$C2ABCD.
; It resolves the selected control record, derives packed coordinate terms from
; its three longwords, and applies frame-local shifts.  No terrain-detail or
; physical-distance interpretation is assigned here.

                org     $C2AB8C

CONTROL_RECORD_BASE             equ     $C46184
CONTROL_RECORD_OFFSET           equ     $C458DE

prepare_map_packet_coordinate_terms:
                lea     CONTROL_RECORD_BASE.l,a4
                adda.w  CONTROL_RECORD_OFFSET.l,a4
                lea     $14(a4),a4
                move.l  4(a4),d2
                addi.l  #$1000,d2
                swap    d2
                rol.l   #4,d2
                neg.l   d2
                move.w  d2,-$2(a6)
                ; Preserve the binary's explicit zero-displacement form.
                dc.w    $202C,0                  ; move.l $0(a4),d0
                move.l  8(a4),d1
                swap    d0
                swap    d1
                move.w  d0,d3
                move.w  d1,d4
                move.w  -$40(a6),d2
                lsr.w   d2,d0
                lsr.w   d2,d1
                tst.w   -$3e(a6)
                bne.b   $C2ABD2
