"""Engine9000 snapshot v8 identity metadata; serialized core bytes stay intact.

Format authority: e9k-debugger/state_buffer.c and snapshot.c in the pinned
Engine9000 source. This module only relocates copies used for replay.
"""
import hashlib
import json
from pathlib import Path
import struct


def checksum(config: bytes) -> int:
    value = 1469598103934665603
    for byte in config:
        value = ((value ^ byte) * 1099511628211) & 0xffffffffffffffff
    return value


def relocate_snapshot(raw: bytes, original_config: bytes, runtime_config: bytes) -> bytes:
    if len(raw) < 40:
        raise ValueError('Truncated Engine9000 snapshot header')
    magic, version, frame, saved_checksum, levels, count = struct.unpack('<8sIQQIQ', raw[:40])
    if magic != b'E9KSNAP\0' or version != 8 or levels != 6:
        raise ValueError('Expected Engine9000 snapshot version 8 with six levels')
    if saved_checksum != checksum(original_config):
        raise ValueError('Snapshot checksum differs from recorded configuration')
    return raw[:20] + struct.pack('<Q', checksum(runtime_config)) + raw[28:]


def prepare_replay_saves(output: Path, original: Path, runtime: Path) -> dict:
    """Adapt identity headers in an already copied save directory, with evidence."""
    before, after = original.read_bytes(), runtime.read_bytes()
    if before == after:
        return {}
    target = output / 'saves' / (original.name + '.e9k-save')
    raw = target.read_bytes()
    derived = relocate_snapshot(raw, before, after)
    target.write_bytes(derived)
    settings = output / 'saves' / (original.name + '.json')
    if settings.is_file():
        data = json.loads(settings.read_text())
        if data.get('rom_checksum') not in (0, checksum(before)):
            raise ValueError('Saved preferences checksum differs from recorded configuration')
        data['rom_checksum'] = checksum(after)
        settings.write_text(json.dumps(data, indent=2) + '\n')
    return {'original_config_sha256': hashlib.sha256(before).hexdigest(),
            'runtime_config_sha256': hashlib.sha256(after).hexdigest(),
            'original_snapshot_sha256': hashlib.sha256(raw).hexdigest(),
            'runtime_snapshot_sha256': hashlib.sha256(derived).hexdigest(),
            'changed_snapshot_bytes': '20..27: config checksum only; all core state bytes unchanged'}
