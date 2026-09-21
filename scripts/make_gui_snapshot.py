"""Package a core save state into the pinned Engine9000 v0.62 snapshot format."""
import argparse
import struct
from pathlib import Path


def package(state, config, frame):
    # snapshot.c's checksum basis is exactly this value (not standard FNV basis).
    checksum = 1469598103934665603
    for byte in config:
        checksum = ((checksum ^ byte) * 1099511628211) & 0xffffffffffffffff
    wrapped = struct.pack('<8s8I', b'E9KSTATE', 2, 40, len(state), 0, 0, 0, 0, 0) + state
    out = struct.pack('<8sIQQIQ', b'E9KSNAP\0', 8, frame, checksum, 6, 1)
    out += struct.pack('<QQ', 1, len(wrapped))
    out += struct.pack('<QQIQQ', 1, frame, 1, len(wrapped), len(wrapped))
    out += wrapped + wrapped
    out += struct.pack('<QQ', 0, 0) * 5
    return out


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('state', type=Path)
    p.add_argument('config', type=Path)
    p.add_argument('output', type=Path)
    p.add_argument('--frame', type=int, default=0)
    a = p.parse_args()
    a.output.parent.mkdir(parents=True, exist_ok=True)
    with a.output.open('xb') as f:
        f.write(package(a.state.read_bytes(), a.config.read_bytes(), a.frame))
