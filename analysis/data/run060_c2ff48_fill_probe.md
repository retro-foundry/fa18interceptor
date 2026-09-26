# run060 `$C2FF48` settled fill probe

The bounded run060 trace reaches `$C2FF48` at engine frame 7991 and enters
`$C301F6` with six screen pairs in `$C4B390`:

```text
(319,100), (208,93), (145,93), (0,115), (0,179), (319,179)
```

The wrapper enables display DMA with `$8400`, then the tuple walker consumes
the six pair records before reaching the renderer submission path. This is a
run060+ settled geometry source for the area route.

The trace and settled Chip snapshots are retained in
`build/run060_c2ff48_fill_probe/`. The settled snapshot includes additional
frame work after the bounded call, so its complete changed range cannot yet be
assigned to this polygon alone. A shorter trace around the individual
`$C304F4` submission, with the same pair list and a one frame settle, is the
next capture needed to isolate the fill colour and edge masks.
