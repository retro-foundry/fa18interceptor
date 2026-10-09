"""Read retained CIA/audio chunks without reading live chipset registers.

Layout authority: pinned UAE savestate.c:restore_chunk, cia.c:save_cia and
audio.c:save_audio. This is reference-host telemetry, not native game behavior.
"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class VoiceReader:
    """Resolve actual Hunk 74 ownership through immutable Hunk 76 code.

    A Workbench restore can relocate these hunks relative to the sealed game
    recording. Unknown/unloaded ownership is explicit, never an empty voice.
    All reads use ordinary RAM; no live registers or serialization are used.
    """
    # Actual C500D8 handler/publication writer PCs, relative to Hunk 76.
    WRITERS = (4, 32, 44, 48, 92, 98, 104, 280, 308)

    def __init__(self, read_memory):
        self.read_memory = read_memory
        inventory = json.loads((ROOT/'analysis/hunk_inventory.json').read_text())
        self.segment = inventory['segments'][76]
        disk = (ROOT/inventory['input']).read_bytes()
        self.disk_sha256 = hashlib.sha256(disk).hexdigest()
        self.raw = disk[self.segment['payload_file_offset']:
                        self.segment['payload_file_offset']+self.segment['size_bytes']]
        manifest = json.loads((ROOT/'analysis/data/romfree_hunk_layout.json').read_text())
        segments = manifest['segments']
        assert hashlib.sha256(self.raw).hexdigest() == next(
            row['disk_payload_sha256'] for row in segments if row['index'] == 76)
        self.relocated = {byte for group in self.segment['reloc32']
                          for offset in group['offsets'] for byte in range(offset,offset+4)}
        assert not self.relocated.intersection(range(24))
        self.layout = None

    @staticmethod
    def in_ram(address, size):
        return 0 <= address <= 0x80000-size or 0xc00000 <= address <= 0xc80000-size

    def candidate(self, base):
        if not self.in_ram(base,len(self.raw)):
            return None
        if self.read_memory(base,24) != self.raw[:24]:
            return None
        loaded = self.read_memory(base,len(self.raw))
        if any(loaded[i] != self.raw[i] for i in range(len(self.raw)) if i not in self.relocated):
            return None
        targets = {}
        for group in self.segment['reloc32']:
            values = {(int.from_bytes(loaded[o:o+4],'big')-
                       int.from_bytes(self.raw[o:o+4],'big')) & 0xffffffff
                      for o in group['offsets']}
            if len(values) != 1:
                return None
            target = group['target_segment']
            value = next(iter(values))
            if target in targets and targets[target] != value:
                return None
            targets[target] = value
        if targets[76] != base or not self.in_ram(targets[74],260) or not self.in_ram(targets[75],404):
            return None
        return {'hunk_74':targets[74], 'hunk_75':targets[75], 'hunk_76':base,
                'voice_slots':targets[74]+16, 'master_volume':targets[74]+254,
                'verified_hunk_76_bytes':len(self.raw),
                'disk_hunk_76_sha256':hashlib.sha256(self.raw).hexdigest()}

    def discover(self):
        matches = []
        for bank in (0,0xc00000):
            data = self.read_memory(bank,0x80000)
            position = data.find(self.raw[:24])
            while position >= 0:
                candidate = self.candidate(bank+position)
                if candidate is not None:
                    matches.append(candidate)
                position = data.find(self.raw[:24],position+1)
        if len(matches) > 1:
            raise RuntimeError('Ambiguous loaded original sound Hunk 76')
        self.layout = matches[0] if matches else None
        return self.layout

    def observe_write(self, source):
        if self.layout is not None or not self.in_ram(source,1):
            return
        for offset in self.WRITERS:
            candidate = self.candidate(source-offset)
            if candidate is not None:
                self.layout = candidate
                return

    def voices(self):
        if self.layout is None:
            return {'voice_layout':None,'voices':None,'master_volume':None}
        result = voice_state(self.read_memory,self.layout['voice_slots'],self.layout['master_volume'])
        result['voice_layout'] = self.layout
        return result


def audio_state(data):
    chunks = {}
    offset = 0
    while offset + 12 <= len(data):
        name = data[offset:offset + 4]
        if name == b'\0\0\0\0':
            offset += 4
            continue
        if name == b'END ':
            break
        size = int.from_bytes(data[offset + 4:offset + 8], 'big')
        if size < 12 or offset + size > len(data):
            raise ValueError('Invalid original state chunk')
        if name in (b'CIAA', b'AUD0', b'AUD1', b'AUD2', b'AUD3'):
            if int.from_bytes(data[offset + 8:offset + 12], 'big') & 1:
                raise ValueError('Compressed original audio/CIA chunk unsupported')
            if name in chunks:
                raise ValueError('Duplicate original audio/CIA chunk')
            chunks[name] = data[offset + 12:offset + size]
        offset += (size + 3) & ~3
    cia = chunks[b'CIAA']
    if len(cia) < 4:
        raise ValueError('Short original CIA chunk')
    result = {'cia_pra': cia[0], 'cia_ddra': cia[2],
              'led_pin_on': not bool((cia[0] | (~cia[2] & 255)) & 2),
              'channels': []}
    for channel in range(4):
        raw = chunks[f'AUD{channel}'.encode()]
        if len(raw) != 25 or not raw[3] & 0x80:
            raise ValueError('Unsupported original audio state layout')
        word = lambda at: int.from_bytes(raw[at:at + 2], 'big')
        long = lambda at: int.from_bytes(raw[at:at + 4], 'big')
        result['channels'].append({'state': raw[0], 'volume': raw[1],
            'interrupt_pending': bool(raw[2]), 'flags': raw[3],
            'length_words': word(4), 'remaining_words': word(6),
            'period': word(8), 'data_word': word(10), 'buffer': long(12),
            'cursor': long(16), 'next_cycles': long(20), 'drhpos': raw[24]})
    return result


def voice_state(read_memory, slots, master):
    # Original C500D8/C50158 owners; reads stay in ordinary chip/slow RAM.
    raw = read_memory(slots, 16)
    voices = []
    for channel in range(4):
        address = int.from_bytes(raw[4 * channel:4 * channel + 4], 'big')
        if address and not (address + 64 <= 0x80000 or
                            0xc00000 <= address <= 0xc80000 - 64):
            raise ValueError('Original voice pointer outside retained game RAM')
        voices.append({'address': address,
                       'record': read_memory(address, 64).hex() if address else None})
    return {'voices': voices, 'master_volume': read_memory(master, 4).hex()}
