; Byte-exact candidate header filters $C26F0C-$C26F27.

                org     $C26F0C

filter_candidate_record_header:
                move.w  (a0,d0.w),d1
                move.w  d1,d5
                andi.w  #$40,d1
                beq.b   $C26F00
                andi.w  #$600,d5
                bne.b   $C26F00
                move.b  $5e(a0,d0.w),d5
                cmp.b   -$1a(a6),d5
                beq.b   $C26F00
