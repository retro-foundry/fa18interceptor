; Byte-exact observed selector-pair table scan $C09AB8-$C09AE9.
; It finds the first selector word in the initial variable-length list, follows
; its signed terminator offset, then scans the second list for the paired word.

                org     $C09AB8

REFRESH_SELECTOR_X               equ     $C45948
REFRESH_SELECTOR_Z               equ     $C4594A

scan_selector_pair_table:
                move.w  REFRESH_SELECTOR_X.l,d0
                move.w  REFRESH_SELECTOR_Z.l,d1
                lea     (a0),a1
                moveq   #-$2,d3
scan_first_selector_word:
                addq.w  #2,d3
                move.w  (a0)+,d2
                blt.b   $C09B44
                cmp.w   d2,d1
                bne.b   scan_first_selector_word
scan_first_selector_tail:
                tst.w   (a0)+
                bge.b   scan_first_selector_tail
                move.w  (a0,d3.w),d3
                lea     (a1,d3.w),a0
                moveq   #-$2,d3
scan_second_selector_word:
                addq.w  #2,d3
                move.w  (a0)+,d2
                blt.b   $C09B44
                cmp.w   d2,d0
                bne.b   scan_second_selector_word
