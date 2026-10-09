"""Validate complete original DMA fetch telemetry against unchanged execution.

Fetched words are retained verbatim. Final-RAM agreement is reported separately;
it never substitutes retained bytes for words actually read by original DMA.
"""
import argparse
from collections import Counter
import gzip
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from original_audio_dma import MAGIC, FRAME, FETCH, FOOTER
from check_original_audio_events import validate_events
from check_original_audio_capture import recorded_bytes


def exact(file, size):
    value = file.read(size)
    if len(value) != size:
        raise ValueError('Truncated original audio DMA stream')
    return value


def open_stream(path):
    return path.open('rb') if path.exists() else gzip.open(str(path)+'.gz','rb')


def frames(file, first_call, last_call):
    if exact(file, len(MAGIC)) != MAGIC:
        raise ValueError('Invalid original audio DMA stream header')
    next_call, previous_source, total = first_call, None, 0
    while True:
        tag = exact(file, 1)
        if tag == b'\0':
            calls, words = FOOTER.unpack(exact(file, FOOTER.size))
            if calls != last_call-first_call+1 or next_call != last_call+1 or words != total or file.read(1):
                raise ValueError('Incomplete original audio DMA footer/coverage')
            return
        if tag != b'\1':
            raise ValueError('Invalid original audio DMA frame tag')
        row = FRAME.unpack(exact(file, FRAME.size))
        call, hardware, source, toggle, hcount, vcount, offset, records, count, samples = row
        if call != next_call or call > last_call or hardware != call:
            raise ValueError('Missing or repeated original audio DMA call')
        if source < 0 or (previous_source is not None and source != previous_source+1):
            raise ValueError('Missing or repeated completed original DMA source frame')
        if toggle not in (0, 1) or (hcount, vcount, records) != (288, 1000, 288000) or abs(offset) >= hcount or count > records:
            raise ValueError('Invalid original audio DMA grid')
        payload = exact(file, count * FETCH.size)
        previous_index = -1
        for index, address, value, register, hpos, vpos, channel in FETCH.iter_unpack(payload):
            if index <= previous_index or index >= records:
                raise ValueError('Missing ordering or duplicated original DMA slot')
            if channel > 3 or register != 0xaa+16*channel or address & 1 or address >= 0x80000:
                raise ValueError('Invalid original DMA channel/register/address')
            if hpos < 0 or not 0 <= vpos < vcount or index != vpos*hcount+hpos+offset:
                raise ValueError('Original DMA beam differs from its grid slot')
            previous_index = index
        yield row, payload
        next_call += 1
        previous_source = source
        total += count


def validate_dma(capture, baseline, pcm_reference=None):
    result = validate_events(capture, baseline, pcm_reference)
    snapshot = json.loads((capture / 'snapshot.json').read_text())
    descriptor = snapshot['audio_dma']
    assert descriptor['collection_mode'] == 6 and descriptor['raw_record_bytes'] == 88 and descriptor['packed_fetch_bytes'] == FETCH.size
    path = capture / descriptor['file']
    with open_stream(path) as file:
        assert hashlib.file_digest(file, 'sha256').hexdigest() == descriptor['sha256']
    audio = snapshot['recorded_audio']
    first, last = audio['first_replay_call'], audio['first_replay_call']+audio['replay_calls']-1
    ends = {}
    for line in (capture / 'audio_chunks.jsonl').read_text().splitlines():
        row = json.loads(line)
        ends[row['call']] = row['first_sample']+row['frames']
    chip = recorded_bytes(capture, 'chip.bin')
    channels, differences, calls, source_frames = [0]*4, [0]*4, 0, []
    first_difference = None
    first_nonzero = [None]*4
    addresses = [Counter() for _ in range(4)]
    with open_stream(path) as file:
        for row, payload in frames(file, first, last):
            call, hardware, source, toggle, hc, vc, offset, records, count, samples = row
            assert samples == ends[call], 'Original DMA/PCM boundary mismatch'
            source_frames.append(source)
            calls += 1
            for index, address, value, register, hpos, vpos, channel in FETCH.iter_unpack(payload):
                channels[channel] += 1
                addresses[channel][address] += 1
                if value and first_nonzero[channel] is None:
                    first_nonzero[channel] = dict(call=call, source_frame=source,
                        address=f'{address:06X}', fetched=value, beam=[hpos,vpos])
                retained = int.from_bytes(chip[address:address+2], 'big')
                if value != retained:
                    differences[channel] += 1
                    if first_difference is None:
                        first_difference = dict(call=call, source_frame=source, channel=channel,
                            address=f'{address:06X}', fetched=value, final_retained=retained,
                            beam=[hpos,vpos])
    assert calls == descriptor['calls'] == result['replay_calls']
    assert channels == descriptor['words_by_channel'] and sum(channels) == descriptor['fetched_words']
    assert source_frames[0] == descriptor['first_source_frame'] and source_frames[-1] == descriptor['last_source_frame']
    retained = path if path.exists() else Path(str(path)+'.gz')
    result.update(audio_dma=descriptor,
        dma_stream_bytes=len(MAGIC)+calls*(1+FRAME.size)+sum(channels)*FETCH.size+1+FOOTER.size,
        dma_retained_bytes=retained.stat().st_size,
        fetched_words_by_channel=channels, unique_fetch_addresses_by_channel=[len(a) for a in addresses],
        first_nonzero_fetched_word_by_channel=first_nonzero,
        final_ram_different_fetched_words_by_channel=differences,
        first_fetched_word_different_from_final_ram=first_difference,
        all_fetched_words_equal_final_ram=not any(differences),
        consecutive_completed_core_frames=True,
        dma_scope='Actual fetched words in every completed original frame view. Boundary PCM, RAM, registers, state and video remain exact; within-block output timing and native waveform acceptance remain open.')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--pcm-reference', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    result = validate_dma(args.capture, args.baseline, args.pcm_reference)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2)+'\n')
    print(f"All {result['audio_dma']['calls']} completed original frames / {result['audio_dma']['fetched_words']} actual fetched words checked; native sound acceptance remains open")


if __name__ == '__main__':
    main()
