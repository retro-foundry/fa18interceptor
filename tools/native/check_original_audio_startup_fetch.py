"""Classify the recorded channel-2 startup prefetch with original functions.

This checks ordered pointer/state/sample use, not beam-to-PCM timing or native
handoff parity. Complete recording validation remains a prerequisite.
"""
import argparse
import copy
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

from check_original_audio_dma import validate_dma, frames, open_stream, FETCH
from check_original_filter_response import function
from check_original_audio_prefix import validate_prefix
from check_original_audio_capture import reference_file, reference_bytes

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'tools/engine9000-src/ami9000/sources/src'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def position(row):
    return row['call'], row['vpos'], row['hpos']


def startup_context(initial, writes, fetched):
    """Require the actual first channel-2 fetch, not a selected zero later on."""
    channel = 2
    state = initial['channels'][channel]
    assert state['state'] == 0 and state['cursor'] == 0 and state['buffer'] == 0
    assert state['length_words'] == 0 and state['remaining_words'] == 0
    assert len(fetched) >= 2
    first, second = fetched[:2]
    before = [row for row in writes if position(row) < position(first)]
    assert all(row['address'] != 0xdff0ca for row in before), 'Manual audio data before startup'
    starts = [row for row in before if row['address'] == 0xdff096 and row['value'] & 0x8004 == 0x8004]
    assert len(starts) == 1, 'Earlier or missing channel-2 DMA enable'
    start, = starts
    assert start['value'] & 0x200, 'Recorded startup must enable DMA master'
    assert not any(position(start) < position(row) and row['address'] == 0xdff096 and
                   not row['value'] & 0x8000 and row['value'] & 4 for row in before), 'DMA disabled before fetch'
    values = {0xdff0c0: 0, 0xdff0c2: 0, 0xdff0c4: 0, 0xdff0c6: state['period'], 0xdff0c8: state['volume']}
    for row in before:
        if row['address'] in values:
            values[row['address']] = row['value']
    pointer = (values[0xdff0c0] << 16) | values[0xdff0c2]
    assert pointer == 108736 and values[0xdff0c4] == 2062
    assert values[0xdff0c6] == 358 and values[0xdff0c8] == 10
    assert (first['call'], first['address'], first['value'], first['hpos'], first['vpos']) == (2178, 0, 0, 21, 64)
    assert (second['call'], second['address'], second['value'], second['hpos'], second['vpos']) == (2178, pointer, 0x21fd, 21, 65)
    # Source case 1 -> 5 is independent of attachment bits; the first payload
    # byte emission in the harness is separately scoped to unmodulated audio.
    return dict(channel=channel, dma_enable=start, first_fetch=first,
                second_fetch=second, published_pointer=pointer,
                published_words=values[0xdff0c4], published_period=values[0xdff0c6],
                published_volume=values[0xdff0c8])


def extracted_header(source):
    structures = [function(source, name) + ';' for name in
                  ('struct audio_channel_data2\n', 'struct audio_channel_data\n')]
    declarations = ('STATIC_INLINE bool usehacks(', 'static void zerostate(',
        'static void update_volume(', 'uae_u16 audio_dmal(', 'static int isirq(',
        'static void setirq(', 'static void newsample(', 'void event_setdsr(',
        'static void setdr(', 'static void loaddat (', 'static void loaddat_1 (',
        'static void loadper1(', 'static void loadperm1(', 'static void loadper (',
        'static bool audio_state_channel2 (', 'uaecptr audio_getpt(')
    bodies = [function(source, declaration) for declaration in declarations]
    constants = '\n'.join(re.findall(r'^#define PERIOD_(?:MIN|LOW)\s+\d+\s*$', source, re.MULTILINE))
    preamble = (constants + '\ntypedef struct { int time, output; } sinc_queue_t;\n' + '\n'.join(structures) +
        '\nstatic struct audio_channel_data audio_channel[AUDIO_CHANNELS_PAULA];\n'
        'static void do_samplerip(struct audio_channel_data *c) { (void)c; abort(); }\n')
    return preamble + '\n\n'.join(bodies) + '\n', declarations


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--pcm-reference', type=Path, required=True)
    parser.add_argument('--entry-prefix', type=Path, required=True,
                        help='Independent replay stopped after call 2177, before channel-2 startup')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    validation = validate_dma(args.capture, args.baseline, args.pcm_reference)
    checkpoint = json.loads((ROOT / 'analysis/figures/native_original_audio_dma_checkpoint.json').read_text())
    assert validation['audio_dma']['sha256'] == checkpoint['audio_dma']['sha256']
    assert validation['authority'] == checkpoint['authority']
    entry = validate_prefix(args.entry_prefix, args.capture, args.pcm_reference)
    assert entry['calls'] == 2177 and entry['endpoint_voice_boundary']['voices'][2]['address'] == 0
    initial, writes = entry['endpoint_hardware'], []
    with reference_file(args.capture / 'audio_events.jsonl') as events:
        for line in events:
            row = json.loads(line)
            if row['kind'] == 'write' and row['call'] <= 2178:
                writes.append(row)
    fetched = []
    with open_stream(args.capture / 'audio_dma.bin') as file:
        for row, payload in frames(file, 1, 21069):
            for index, address, value, register, hpos, vpos, channel in FETCH.iter_unpack(payload):
                if channel == 2 and len(fetched) < 2:
                    fetched.append(dict(call=row[0], source_frame=row[2], address=address,
                        value=value, hpos=hpos, vpos=vpos, grid_index=index, register=register))
    context = startup_context(initial, writes, fetched)
    controls = []
    def rejected(name, state, events, words):
        try:
            startup_context(state, events, words)
        except AssertionError:
            controls.append(name)
        else:
            raise AssertionError('Accepted corrupt startup context: ' + name)
    corrupt = copy.deepcopy(initial)
    corrupt['channels'][2]['state'] = 2
    rejected('already-playing channel', corrupt, writes, fetched)
    corrupt = copy.deepcopy(fetched)
    corrupt[0]['address'] = 108736
    rejected('lost old-pointer fetch', initial, writes, corrupt)
    corrupt = copy.deepcopy(fetched)
    corrupt[1]['value'] ^= 1
    rejected('changed actual payload word', initial, writes, corrupt)
    rejected('earlier DMA enable', initial, writes + [dict(call=1, vpos=1, hpos=1,
        address=0xdff096, value=0x8204)], fetched)
    rejected('manual data before first DMA', initial, writes + [dict(call=1, vpos=1, hpos=1,
        address=0xdff0ca, value=0x1234)], fetched)
    source = (SOURCE / 'audio.c').read_text()
    for file, name, value in (('sysdeps.h', 'CYCLE_UNIT', 512), ('custom.h', 'DMA_MASTER', 512),
                              ('audio.h', 'AUDIO_CHANNELS_PAULA', 4)):
        match = re.search(r'^#define\s+' + name + r'\s+(0x[0-9a-fA-F]+|\d+)\b',
                          (SOURCE / 'include' / file).read_text(), re.MULTILINE)
        assert match and int(match[1], 0) == value
    header, declarations = extracted_header(source)
    compiler = shutil.which('gcc')
    assert compiler
    args.out.parent.mkdir(parents=True, exist_ok=True)
    harness = ROOT / 'tools/native/original_audio_startup_fetch.c'
    with tempfile.TemporaryDirectory(prefix='original-startup-', dir=args.out.parent) as directory:
        work = Path(directory)
        (work / 'original_audio_startup.h').write_text(header)
        executable = work / 'startup.exe'
        subprocess.run([compiler, '-O2', '-Wall', '-Wextra', '-Werror',
                        '-Wno-unused-parameter', '-Wno-unused-variable', '-I' + str(work),
                        str(harness), '-o', str(executable)], check=True)
        result = subprocess.run([str(executable), '2', str(context['published_pointer']),
            str(context['first_fetch']['value']), str(context['second_fetch']['value'])],
            check=True, capture_output=True, text=True)
        oracle = json.loads(result.stdout)
        executable_sha256 = sha(executable.read_bytes())
    report = dict(scope='Recorded channel-2 idle startup old-pointer fetch is discarded by unchanged '
        'original functions. Ordered state/sample use only; beam-to-PCM timing, native onset/handoffs '
        'and the channel-1 call-94 zero word remain open.',
        authority=validation['authority'], dma_stream_sha256=validation['audio_dma']['sha256'],
        event_log_sha256=sha(reference_bytes(args.capture / 'audio_events.jsonl')),
        source_sha256=sha((SOURCE / 'audio.c').read_bytes()),
        extracted_header_sha256=sha(header.encode()), extracted_functions=list(declarations),
        harness_sha256=sha(harness.read_bytes()), reference_executable_sha256=executable_sha256,
        compiler=subprocess.run([compiler, '--version'], check=True, capture_output=True, text=True).stdout.splitlines()[0],
        actual_context=context, source_oracle=oracle, rejected_controls=controls,
        actual_entry_prefix=entry,
        classification='startup prefetch, not a sample-buffer word',
        native_runtime_changed=False, native_waveform_accepted=False,
        remaining_uncatalogued_fetch=dict(channel=1, call=94, address=0x25a72, value=0, beam=[19, 277]))
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print('Original channel-2 startup prefetch discarded; 66,560 poison/attachment cases and five context controls pass. Native sound timing remains open.')


if __name__ == '__main__':
    main()
