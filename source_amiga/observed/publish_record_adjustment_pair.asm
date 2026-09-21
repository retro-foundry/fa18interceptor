; Byte-exact observed adjustment publication $C2DDB2-$C2DDBF.

                org     $C2DDB2

publish_record_adjustment_pair:
                move.w  d4,$22(a1)
                move.w  d5,$24(a1)
                move.w  d4,d3
                bge.b   $C2DDC0
