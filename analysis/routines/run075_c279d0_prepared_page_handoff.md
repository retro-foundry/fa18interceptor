# run075 prepared-page `$C279D0` native handoff

Classification: **scenario-backed renderer input boundary**.

The first `$C279D0` invocation in the prepared-page interval occurs at local
trace frame 184 (global run075 frame 384) and returns after 4,593 instructions.
It is the invocation that targets the four-plane render-page pointer family;
the later visible frame selects the already prepared five-plane page.

At the entry packet's normal route, the source stores the three projection
inputs in its stack locals before selecting the original Hunk-25 `$C28124`
table:

| Source value | Frame-384 observed value | Native owner boundary |
| --- | ---: | --- |
| `$C45A72.w` | `$E7C1` | first grid component |
| `$C45A76.w` | `$E64E` | second grid component |
| `$C45A78.l` | `$FFFFFF83` | projection/depth component |
| `$C45BD8` | read by each projected record | nine-word renderer matrix |

The `$C27A54` trace row reads the published depth low word `$FF83`, proving
that the renderer packet consumes the projection publisher's output rather
than a frame counter. It then selects table `$C28124` because that signed
value is at least `-$80`; the exact 96-record Hunk-25 table is already loaded
by `FA18ProjectionGrid`.

`$0004` and `$F800` occur as `$C279D0`'s own later control values. They are
not the `$C45A72/$C45A76` packet. The saved slow-RAM bytes and the trace's
post-read register values agree on the signed packet above; the same tuple is
also present at the global-frame-392 first renderer invocation. This keeps
the fade-stage evidence separate from renderer-local setup values.

Thus the missing native composition is:

```text
scene/root transform -> projection packet ($C45A72/$76/$78)
                         + matrix ($C45BD8)
                       -> FA18ProjectionGrid pass
                       -> five-plane render page
                       -> existing Copper page presentation
```

The trace also proves that this must be data-driven: the top-level call arrives
with unrelated register values, then the packet forms its own inputs from the
published workspace. The frame-384 values above are an oracle for integration
tests, not constants permitted in the runtime path.
