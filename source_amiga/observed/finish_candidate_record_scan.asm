; Byte-exact no-candidate scan exit $C27504-$C2751B.

                org     $C27504

CANDIDATE_RECORD_BASE           equ     $C46184
CANDIDATE_RECORD_OFFSET          equ     $C459B6
RETURN_CANDIDATE_SCAN_STAGE      equ     $C279BC

finish_candidate_record_scan:
                lea.l   CANDIDATE_RECORD_BASE.l,a3
                adda.w  CANDIDATE_RECORD_OFFSET.l,a3
                cmpi.l  #$7fff,$10(a3)
                bgt.w   RETURN_CANDIDATE_SCAN_STAGE
