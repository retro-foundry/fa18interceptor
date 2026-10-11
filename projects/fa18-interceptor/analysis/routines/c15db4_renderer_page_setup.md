# `$C15DB4-$C1601E`: five-plane renderer-page setup

Classification: **complete allocation and pointer-layout prefix**.

The cold-boot source routine begins at `$C15DB4`. Its proved allocation path
creates the five-page family later observed at `$04DB30,$04FA70,$0519B0,
$0538F0,$055830`:

1. `$C15ED0-$C15F36` calls the allocator four times with size `$1F40` and
   stores each result in both the `$C18272` page record and
   `$C456BE/$C456C2/$C456C6`.
2. `$C15F3C-$C15FA8` allocates and stores the fourth and fifth planes at
   `$C456CA/$C456CE`, with the source's failure checks between allocations.
3. `$C15FC0-$C15FDE` duplicates the lower four plane pointers at
   `$C456D2/$C456D6/$C456DA/$C456DE`.
4. `$C15FE8-$C16018` copies the first four plane pointers into the second
   display record; `$C1601E` calls `$C2F4DE`, whose byte-exact source builds
   the two renderer pointer-table orders.

At cold-boot frame 5,926, both `$C18272` and `$C456BE` change from all zeroes
to the same five 8,000-byte plane pointers. This is the actual dynamic
five-plane render-page allocation, unlike the adjacent startup `pix/splsh`
resource setup.

`renderer_page_setup.{c,h}` ports the allocation result as one native private
40,000-byte page. It initializes a `FA18FivePlaneChipBinding`, derives the
five source offsets and duplicated lower-four sources, and applies the exact
`$C2F4DE` table order. No original Chip-RAM address, replay page, or frame
scheduler is imported. The surrounding graphics/ViewPort construction,
palette copy, and callback timing remain caller-owned.
