; Byte-exact candidate-record scan entry $C26EBE-$C26EFB.

                org     $C26EBE

CANDIDATE_SCAN_STATUS           equ     $C4589F ; set by later broad-phase acceptance
CANDIDATE_RECORD_BASE           equ     $C46184
CANDIDATE_RECORD_OFFSET          equ     $C459B6
RETURN_CLASS20_CANDIDATE_SCAN    equ     $C26EB8

initialize_candidate_record_scan:
                clr.b   CANDIDATE_SCAN_STATUS.l
                moveq   #0,d0
                link.w  a6,#-$5c
                lea.l   CANDIDATE_RECORD_BASE.l,a0
                move.w  CANDIDATE_RECORD_OFFSET.l,d0
                move.b  $5e(a0,d0.w),-$1a(a6)
                andi.b  #$3f,$4(a0,d0.w)
                move.w  (a0,d0.w),-$14(a6)
                move.w  $2(a0,d0.w),-$16(a6)
                move.b  $62(a0,d0.w),d6
                andi.b  #$f0,d6
                cmpi.b  #$20,d6
                beq.b   RETURN_CLASS20_CANDIDATE_SCAN
