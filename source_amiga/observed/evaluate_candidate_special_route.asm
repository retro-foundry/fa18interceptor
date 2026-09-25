; Byte-exact candidate special-route body $C27234-$C272AB.
; It applies a two-component signed product gate to offsets in the candidate
; record stream; candidate identity and geometric interpretation remain open.

                org     $C27234

CANDIDATE_RECORD_BASE           equ     $C46184
CANDIDATE_RECORD_OFFSET         equ     $C459B6

evaluate_candidate_special_route:
                move.w  #$1E8,-$46(a6)
                move.w  #$1EE,-$48(a6)
                move.w  #$1F4,-$4a(a6)
                lea     CANDIDATE_RECORD_BASE.l,a4
                adda.w  CANDIDATE_RECORD_OFFSET.l,a4
                tst.w   -$44(a6)
                beq.w   $C273A0
                move.w  2(a4),d2
                andi.w  #$80,d2
                beq.w   $C27388
                move.w  -$46(a6),d0
                move.w  -$4a(a6),d1
                move.w  (a3,d0.w),d6
                move.w  4(a3,d0.w),d7
                sub.w   (a3,d1.w),d6
                sub.w   4(a3,d1.w),d7
                neg.w   d7
                move.w  d6,d2
                move.w  d7,d3
                move.w  (a3,d1.w),d4
                move.w  4(a3,d1.w),d5
                move.w  -$5a(a6),d1
                asr.w   d1,d4
                asr.w   d1,d5
                add.w   $c(a3),d4
                add.w   $e(a3),d5
                sub.w   a2,d4
                sub.w   -$42(a6),d5
                muls.w  d4,d7
                muls.w  d5,d6
                add.l   d7,d6
                blt.w   $C27388
