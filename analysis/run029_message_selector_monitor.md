# Run029 message-selector monitor

`build/run029_selector74_first5000/report.json` records a normal replay of
run029 frames 1--5,000 with a breakpoint at `$C32D24`, the message-record
selector. It observes these selector values and caller return addresses:

| Frame | Selector | Static payload | Return |
|---:|---:|---|---|
| 270 | 105 | `5 ... QUALIFICATION: REQUIRED FOR MISSIONS` | `$C0F3C0` |
| 431 | 71 | `CRACKED BY A-HA` | `$C0F3C0` |
| 471 | 4 | whitespace record | `$C0F3C0` |
| 501 | 73 | `QUALIFICATION` | `$C0F3C0` |
| 550 | 90 | `READY TO LAUNCH` | `$C0F3C0` |

`build/run029_selector74_after5000/report.json` observes no `$C32D24`
invocation through the pre-close replay window. Selector 74 (`LANDING
SUCCESSFUL`) is not observed by this particular message-record path. This is
consistent with run029's completion now being unconfirmed; no success display
may be attributed to `$C32D24` or to this capture.

The monitor preserves the register state at a target selector without stepping
after it. It establishes selector values and caller return addresses only; it
does not identify the success-message producer or status writer.
