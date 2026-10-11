"""Compare startup music or complete-recording retained sample payloads.

Matching uses original sample bytes and ordered requests, not relocated pointer
values, a searched subsequence or a shifted WAV. The optional full catalog
checks every request's retained payload on its actual channel, without claiming
fetch-time contents or ordered handoffs. Neither mode accepts complete sound.
"""
import argparse
import array
from collections import Counter
import gzip
import hashlib
import json
from pathlib import Path
import wave

from check_native_audio_trace import read_audio_trace
from check_original_audio_events import validate_events
from check_original_audio_capture import recorded_bytes, reference_file
from check_original_audio_samples import validate_samples, frames, sample_records, open_stream, CYCLE_UNIT


def digest(data):
    return hashlib.sha256(data).hexdigest()


def requests(folder,layout):
    banks=[recorded_bytes(folder,name) for name in ('chip.bin','slow.bin')]
    registers=[{} for _ in range(4)]
    pending=[None]*4
    result=[[] for _ in range(4)]
    with reference_file(folder/'audio_events.jsonl') as log:
        for line in log:
            row = json.loads(line)
            if row.get('kind')!='write' or not 0xdff0a0<=row['address']<=0xdff0da:
                continue
            channel,register=divmod(row['address']-0xdff0a0,16)
            registers[channel][register]=row['value']
            offset=row['source']-layout['hunk_76']
            if offset==44:
                assert register==4 and pending[channel] is None
                pointer=(registers[channel][0]<<16|registers[channel][2])&~1
                size=2*(row['value'] or 65536)
                bank,at=(banks[0],pointer) if pointer<0x80000 else (banks[1],pointer-0xc00000)
                payload=bank[at:at+size]
                assert at>=0 and len(payload)==size
                pending[channel]={'call':row['call'],'hardware_frame':row['hardware_frame'],
                    'vpos':row['vpos'],'hpos':row['hpos'],'samples':pointer,
                    'bytes':size,'sha256':digest(payload)}
            elif offset==308 and pending[channel] is not None:
                assert register==8
                pending[channel].update(period=registers[channel][6],volume=row['value'])
                result[channel].append(pending[channel]);pending[channel]=None
    assert all(item is None for item in pending)
    return result


def compare(original,native):
    for channel,events in enumerate(original):
        assert len(native[channel])>=len(events)
        for index,event in enumerate(events):
            actual=native[channel][index]
            assert all(event[field]==actual[field] for field in ('bytes','sha256','period','volume')),(channel,index)


def compare_payload_catalog(original,native,request_count):
    """Compare every retained payload on its actual channel, without alignment.

    Original pointer/length comes from recorded writes; its payload is read
    from final retained RAM. This does not prove fetch-time immutability or
    equal request order, repetition count, period, volume or waveform timing.
    """
    assert sum(map(len,original))==request_count, 'Original handler request coverage changed'
    assets=[{(event['bytes'],event['sha256']) for event in channel} for channel in native]
    for channel,events in enumerate(original):
        assert events and assets[channel], ('Missing complete channel evidence',channel)
        for index,event in enumerate(events):
            assert (event['bytes'],event['sha256']) in assets[channel], ('Unknown channel payload',channel,index)


def complete_payload_catalog(original,native,validation,report,native_data):
    request_count=validation['source_writes'][f"{validation['resolved_voice_layout']['hunk_76']+44:06X}"]
    compare_payload_catalog(original,native,request_count)
    rejected=[]
    for change in ('length','payload','channel','missing-request'):
        damaged=[[dict(event) for event in channel] for channel in original]
        if change=='length': damaged[0][0]['bytes']+=2
        elif change=='payload': damaged[0][0]['sha256']='0'*64
        elif change=='channel': damaged[3].append(damaged[0].pop(0))
        else: damaged[0].pop()
        try: compare_payload_catalog(damaged,native,request_count)
        except AssertionError: rejected.append(change)
        else: raise AssertionError(f'Accepted damaged full payload catalog: {change}')
    catalogs=[]
    for channel in range(4):
        identities=sorted({(event['bytes'],event['sha256']) for event in original[channel]})
        catalogs.append([dict(bytes=size,sha256=checksum,
            original_requests=sum((e['bytes'],e['sha256'])==(size,checksum) for e in original[channel]),
            native_requests=sum((e['bytes'],e['sha256'])==(size,checksum) for e in native[channel]),
            original_sample_addresses=sorted({e['samples'] for e in original[channel] if (e['bytes'],e['sha256'])==(size,checksum)}),
            native_sample_addresses=sorted({e['samples'] for e in native[channel] if (e['bytes'],e['sha256'])==(size,checksum)}))
            for size,checksum in identities])
    return dict(scope='Every complete original recorded handler request resolves in retained final RAM to a native-owned payload on the same channel. No fetched-sample lifetime, ordered handoff, repetition, level, pitch, onset or waveform parity is inferred.',
        original_validation=validation,native_runner_sha256=report['runner_sha256'],
        native_trace_sha256=report['complete_audio_trace_sha256'],native_final_ram_sha256=digest(native_data),
        original_requests_by_channel=[len(c) for c in original],native_active_requests_by_channel=[len(c) for c in native],
        original_total_requests=request_count,
        original_unique_payloads=len({(e['bytes'],e['sha256']) for c in original for e in c}),
        native_unique_payloads=len({(e['bytes'],e['sha256']) for c in native for e in c}),
        all_retained_original_requested_payloads_match_native_on_same_channel=True,
        payload_catalog_by_channel=catalogs,negative_controls_rejected=rejected,
        alignment_search_shift_trim_gain_or_clock_change=False,whole_flight_sound_acceptance=False)


def onset(path):
    with reference_file(path) as file, wave.open(file) as wav:
        rate=wav.getframerate()
        samples=array.array('h',wav.readframes(wav.getnframes()))
    first=[next((i//2 for i,value in enumerate(samples) if i%2==channel and value),None)
           for channel in (0,1)]
    assert all(value is not None for value in first)
    return {'rate_hz':rate,'first_nonzero_stereo_frame_by_channel':first,
            'right_minus_left_frames':first[1]-first[0]}


def native_recording_rate(folder, report):
    # The WAV is the actual host output contract. Bind the complete bytes and
    # frame count before interpreting native trace timestamps at that rate.
    with reference_file(folder/'native.wav') as file:
        assert hashlib.file_digest(file, 'sha256').hexdigest() == report['wav_sha256'], 'Native PCM identity differs'
    with reference_file(folder/'native.wav') as file, wave.open(file) as pcm:
        rate = pcm.getframerate()
        assert rate in (44100, 48000), 'Unqualified native output rate'
        assert (pcm.getnchannels(), pcm.getsampwidth(), pcm.getnframes()) == (2, 2, report['stats']['sample_frames'])
    assert report['stats']['sample_frames'] == report['stats']['frames'] * (rate//50), 'Native output duration differs'
    assert report.get('sample_rate_hz', rate) == rate, 'Declared native output rate differs'
    return rate


def consumed_rows(folder):
    with open_stream(folder/'audio_samples.bin') as file:
        for (call, count, before, after, pcm_end), payload in frames(file, 5000):
            for row in sample_records(payload, before, after):
                yield (call, *row)


def consume_requests(original, chip, rows):
    """Check every observed byte against published buffers, without alignment."""
    positions = [[0, 0] for _ in range(4)]
    buffers = [[] for _ in range(4)]
    cycles = [[] for _ in range(4)]
    first = [None]*4
    first_nonzero = [None]*4
    previous = [None]*4
    for row in rows:
        call, cycle, address, word, value, channel, state, hpos, vpos = row
        index, cursor = positions[channel]
        assert index < len(original[channel]), 'Consumed byte has no published buffer'
        request = original[channel][index]
        expected_address = request['samples'] + (cursor & ~1)
        assert address == expected_address, 'Consumed word address differs from published buffer'
        assert word == int.from_bytes(chip[address:address+2], 'big'), 'Consumed word differs from published payload'
        assert value & 255 == chip[request['samples']+cursor], 'Consumed byte differs from published payload'
        assert state == (3 if cursor & 1 else 2), 'Consumed high/low-byte order differs'
        if previous[channel] is not None:
            before, period = previous[channel]
            assert cycle-before == period*CYCLE_UNIT, 'Consumed byte interval differs from published period'
        previous[channel] = cycle, request['period']
        cycles[channel].append(cycle)
        observed = dict(call=call, cycle=cycle, address=f'{address:06X}', value=value, beam=[hpos, vpos])
        if first[channel] is None:
            first[channel] = observed
        if value and first_nonzero[channel] is None:
            first_nonzero[channel] = observed
        if not cursor:
            buffers[channel].append(dict(request=index, bytes=request['bytes'], consumed=0,
                first_call=call, first_cycle=cycle, complete=False))
        buffers[channel][-1]['consumed'] += 1
        cursor += 1
        if cursor == request['bytes']:
            buffers[channel][-1]['complete'] = True
            index, cursor = index+1, 0
        positions[channel] = index, cursor
    assert cycles[0] and cycles[1] and not cycles[2] and not cycles[3], 'Different startup channel coverage'
    phases = Counter(b-a for a, b in zip(cycles[0], cycles[1]))
    assert all(phase % CYCLE_UNIT == 0 for phase in phases), 'Nonintegral recorded chip-clock phase'
    return dict(bytes_by_channel=[len(c) for c in cycles], first_samples_by_channel=first,
        first_nonzero_samples_by_channel=first_nonzero, consumed_buffers_by_channel=buffers,
        complete_buffers_by_channel=[sum(b['complete'] for b in channel) for channel in buffers],
        right_minus_left_chip_clocks={str(phase//CYCLE_UNIT):count for phase, count in phases.items()},
        all_consumed_bytes_addresses_order_and_periods_match_published_buffers=True,
        scope='Complete observed startup byte sequence against original published buffers and periods. '
            'Relative channel phase is observed, never supplied to native playback. '
            'No separate DMA coverage or complete native waveform acceptance.')


def assess_consumption(folder, baseline, pcm_reference, original):
    validation = validate_samples(folder, baseline, pcm_reference, reference_context='startup')
    chip = recorded_bytes(folder, 'chip.bin')
    report = consume_requests(original, chip, consumed_rows(folder))
    assert report['bytes_by_channel'] == validation['bytes_by_channel']
    # Change actual source records. Rehashing a descriptor must not excuse a
    # word/address/period error, nor omission of a byte at a buffer handoff.
    prefix, channel_zero_bytes, handoff_index = [], 0, None
    for row in consumed_rows(folder):
        if row[5] == 0:
            channel_zero_bytes += 1
            if channel_zero_bytes == original[0][0]['bytes']+1:
                handoff_index = len(prefix)
        prefix.append(row)
        if channel_zero_bytes == original[0][0]['bytes']+3:
            break
    assert handoff_index is not None, 'No actual startup buffer handoff'
    guards = {}
    for kind in ('word_and_byte', 'word_address', 'byte_interval', 'missing_handoff_byte'):
        changed = list(prefix)
        if kind == 'missing_handoff_byte':
            changed.pop(handoff_index)
        else:
            position = 1 if kind == 'byte_interval' else 0
            row = list(changed[position])
            if kind == 'word_and_byte':
                row[3] ^= 0x100; row[4] ^= 1
            elif kind == 'word_address':
                row[2] += 2
            else:
                row[1] += CYCLE_UNIT
            changed[position] = tuple(row)
        try:
            consume_requests(original, chip, changed)
        except AssertionError:
            guards[kind] = True
        else:
            raise AssertionError(f'Accepted altered consumed sample: {kind}')
    report.update(validation=validation, mutation_rejections=guards)
    return report


def startup_dispatch(slow, layout, writes, original, consumed, native):
    """Observe startup dispatch latency; never turn it into a host delay.

    All compared events are in one recorded hardware frame, so no frame-line
    count, CPU cycle model, output resampling or fitted clock is required.
    Caller/interrupt/bus work is not attributed to the empty-voice loop alone.
    """
    handler = layout['hunk_76']
    empty_at = handler + 92 - 0xc00000
    # MOVE.W #0,8(A0); MOVE.W #124,6(A0); DMA off via descriptor;
    # MOVE.W #400,D0; SUBI.W #1,D0; BNE back to SUBI.
    empty_code = bytes.fromhex('317c00000008317c007c000633e9001200dff096303c01900440000166fa')
    assert slow[empty_at:empty_at+len(empty_code)] == empty_code, 'Different empty-voice ISR/wait'
    trigger = layout['hunk_75'] + 136
    channels = []
    for channel in (0, 1):
        first = consumed['first_samples_by_channel'][channel]
        irq, dma = 0x80 << channel, 1 << channel
        base = 0xdff0a0 + channel*16
        rows = [row for row in writes if row['call'] == first['call'] and (
            row['source'] == trigger and row['address'] == 0xdff09c and row['value'] == irq|0x8000 or
            row['source'] == handler+4 and row['address'] == 0xdff09c and row['value'] == irq or
            handler <= row['source'] < handler+420 and base <= row['address'] <= base+8 or
            row['source'] in (handler+48, handler+104) and row['address'] == 0xdff096 and
                row['value'] in (dma, dma|0x8200))]
        enable = next(i for i, row in enumerate(rows) if row['source'] == handler+48)
        assert enable >= 10, 'Missing empty/start interrupt prefix'
        rows = rows[enable-10:]
        request = original[channel][0]
        assert original[channel][1]['samples'] == request['samples'], 'Different priming buffer'
        assert all(request[k] == original[channel][1][k] for k in ('bytes', 'period', 'volume'))
        publication = [(32, base, request['samples'] >> 16),
            (32, base+2, request['samples'] & 65535), (44, base+4, request['bytes']//2),
            (48, 0xdff096, dma|0x8200), (280, base+6, request['period']),
            (308, base+8, request['volume'])]
        expected = [(trigger-handler, 0xdff09c, irq|0x8000), (4, 0xdff09c, irq),
            (92, base+8, 0), (98, base+6, 124), (104, 0xdff096, dma),
            (trigger-handler, 0xdff09c, irq|0x8000), (4, 0xdff09c, irq),
            *publication, (4, 0xdff09c, irq), *publication]
        assert [(r['source']-handler, r['address'], r['value']) for r in rows] == expected, 'Different startup interrupt/write sequence'
        assert len({r['hardware_frame'] for r in rows}) == 1, 'Startup dispatch spans hardware frames'
        beam_clock = lambda r: r['vpos']*227 + r['hpos']
        positions = [beam_clock(row) for row in rows]
        assert all(b > a for a, b in zip(positions, positions[1:])), 'Nonmonotonic startup write beam'
        first_clock = first['beam'][1]*227 + first['beam'][0]
        assert positions[12] < first_clock < positions[14], 'Sample does not follow initial publication before priming pointer'
        labels = ['empty_request', 'empty_ack', 'silence', 'minimum_period', 'dma_off',
            'start_request', 'start_ack', 'pointer_high', 'pointer_low', 'length',
            'dma_on', 'period', 'volume', 'prime_ack', 'prime_pointer_high',
            'prime_pointer_low', 'prime_length', 'prime_dma_on', 'prime_period', 'prime_volume']
        channels.append(dict(channel=channel, call=first['call'], hardware_frame=rows[0]['hardware_frame'],
            writes=[dict(event=label, source=f"{row['source']:06X}", address=f"{row['address']:06X}",
                value=row['value'], beam=[row['hpos'], row['vpos']]) for label, row in zip(labels, rows)],
            first_sample=first, observed_chip_clocks=dict(
                empty_dma_off_to_start_request=positions[5]-positions[4],
                start_request_to_ack=positions[6]-positions[5],
                start_ack_to_dma_on=positions[10]-positions[6],
                dma_on_to_first_sample=first_clock-positions[10]),
            dma_on_clock=positions[10], first_sample_clock=first_clock,
            native_initial_output_frame=native[channel][0]['sample_frame']))
    assert channels[0]['hardware_frame'] == channels[1]['hardware_frame'], 'Channels start in different frames'
    dma_phase = channels[1]['dma_on_clock']-channels[0]['dma_on_clock']
    fetch_phase = channels[1]['observed_chip_clocks']['dma_on_to_first_sample']-channels[0]['observed_chip_clocks']['dma_on_to_first_sample']
    phase = channels[1]['first_sample_clock']-channels[0]['first_sample_clock']
    assert dma_phase+fetch_phase == phase
    assert set(consumed['right_minus_left_chip_clocks']) == {str(phase)}, 'Initial dispatch does not explain complete observed channel phase'
    return dict(scope='Recorded empty/start/prime interrupt sequence and source wait instructions. Same-frame beam subtraction only; observed spans include caller, interrupt and bus work. No CPU-cycle attribution or portable delay is inferred.',
        empty_voice_source=f'{handler+92:06X}', empty_voice_bytes=empty_code.hex(),
        empty_voice_wait_iterations=400, channels=channels,
        right_minus_left_chip_clocks=dict(dma_enable=dma_phase, following_fetch=fetch_phase, first_sample=phase),
        native_right_minus_left_initial_output_frames=channels[1]['native_initial_output_frame']-channels[0]['native_initial_output_frame'],
        runtime_delay_or_clock_change=False, whole_flight_sound_acceptance=False)


def assess_startup_dispatch(folder, layout, original, consumed, native):
    slow = recorded_bytes(folder, 'slow.bin')
    with reference_file(folder/'audio_events.jsonl') as log:
        writes = [row for line in log if (row := json.loads(line)).get('kind') == 'write']
    report = startup_dispatch(slow, layout, writes, original, consumed, native)
    rejected = []
    for kind in ('wait_count', 'interrupt_source', 'dma_value', 'beam_order'):
        changed_slow, changed_writes = slow, [dict(row) for row in writes]
        if kind == 'wait_count':
            data = bytearray(slow); data[layout['hunk_76']+115-0xc00000] ^= 1
            changed_slow = bytes(data)
        else:
            at = next(i for i, r in enumerate(changed_writes) if r['source'] == layout['hunk_76']+48)
            if kind == 'interrupt_source': changed_writes[at]['source'] += 2
            elif kind == 'dma_value': changed_writes[at]['value'] ^= 1
            else: changed_writes[at]['vpos'] -= 2
        try:
            startup_dispatch(changed_slow, layout, changed_writes, original, consumed, native)
        except (AssertionError, StopIteration): rejected.append(kind)
        else: raise AssertionError(f'Accepted altered startup dispatch: {kind}')
    report['mutation_rejections'] = rejected
    return report


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original',type=Path,required=True)
    parser.add_argument('--baseline',type=Path,required=True)
    parser.add_argument('--native-trace',type=Path,required=True)
    parser.add_argument('--native-report',type=Path,required=True)
    parser.add_argument('--complete-payload-catalog',action='store_true',
                        help='Check every complete recording request against same-channel retained native payloads; no handoff/timing acceptance')
    parser.add_argument('--native-data',type=Path,
                        help='Verified native final RAM required for the complete payload catalog')
    parser.add_argument('--pcm-reference',type=Path,
                        help='Explicit reusable original WAV; still checked against every original PCM block/hash')
    parser.add_argument('--consumed-samples', type=Path,
                        help='Verified independent startup observer; compare every consumed byte and buffer handoff')
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    if args.complete_payload_catalog != (args.native_data is not None):
        parser.error('--complete-payload-catalog requires --native-data; native data is only used with that mode')
    if args.complete_payload_catalog and args.consumed_samples:
        parser.error('--consumed-samples requires the ordered startup comparison')
    validation=validate_events(args.original,args.baseline,args.pcm_reference)
    report=json.loads(args.native_report.read_text())
    trace=gzip.decompress(args.native_trace.read_bytes()) if args.native_trace.suffix=='.gz' else args.native_trace.read_bytes()
    assert digest(trace)==report['complete_audio_trace_sha256']
    native_data=None
    if args.native_data:
        native_data=gzip.decompress(args.native_data.read_bytes()) if args.native_data.suffix=='.gz' else args.native_data.read_bytes()
        assert len(native_data)==0x100000
        assert digest(native_data)==report['final_data_retention']['decoded_sha256']
    native_rate = native_recording_rate(args.native_report.parent, report)
    read_audio_trace(args.native_trace,report['stats'],native_data, frames_per_tick=native_rate//50)
    native=[[] for _ in range(4)]
    for line in trace.splitlines():
        row=json.loads(line)
        if row.get('kind')=='request' and row['active']:
            native[row['channel']].append(row)
    original=requests(args.original,validation['resolved_voice_layout'])
    if args.complete_payload_catalog:
        result=complete_payload_catalog(original,native,validation,report,native_data)
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(result,indent=2)+'\n')
        print(f"All {result['original_total_requests']} original requests resolve to retained native payloads on the same channel; four mutations rejected; handoff/timing acceptance remains open")
        return
    assert sum(map(len,original))==validation['source_writes'][f"{validation['resolved_voice_layout']['hunk_76']+44:06X}"]
    assert all(original[channel] for channel in (0,1)) and not original[2] and not original[3]
    compare(original,native)
    for field,value in (('bytes',2),('sha256','0'*64),('period',124),('volume',0)):
        damaged=[[dict(event) for event in channel] for channel in original]
        damaged[0][2][field]=value
        try: compare(damaged,native)
        except AssertionError: pass
        else: raise AssertionError(f'Accepted changed {field}')
    timing={}
    for channel in (0,1):
        events=original[channel]
        # Report alternative beam-line interpretations. This is diagnostic
        # inference about the reference recording, not a native clock change.
        residuals={}
        for lines in (312,313,314):
            values=[]
            for index in range(2,len(events)-1):
                before,after=events[index:index+2]
                assert before['period']==after['period']==358
                cycles=((after['hardware_frame']-before['hardware_frame'])*lines+
                        after['vpos']-before['vpos'])*227+after['hpos']-before['hpos']
                values.append(cycles-events[index-1]['bytes']*358)
            residuals[str(lines)]={'intervals':len(values),'min_cycles':min(values),'max_cycles':max(values)}
        native_error=[]
        for index in range(2,len(events)):
            expected=sum(event['bytes']*358 for event in events[:index-1])
            actual=native[channel][index]['sample_frame']*3546895/native_rate
            native_error.append(actual-expected)
        timing[str(channel)]={'beam_line_candidates':residuals,
            'native_refill_rounding_cycles':{'min':min(native_error),'max':max(native_error)},
            'original_initial_and_prime':events[:2],
            'native_initial_and_prime_output_frames':[native[channel][i]['sample_frame'] for i in (0,1)]}
    result={'scope':'Every original initial-music request in the separate 5000-call launch versus native ordered startup requests; onset and reference-clock differences reported, not accepted as identical WAVs',
        'original_validation':validation,'native_runner_sha256':report['runner_sha256'],
        'native_trace_sha256':digest(trace),'requests_by_channel':[len(channel) for channel in original],
        'native_output_rate_hz':native_rate,
        'all_original_requests_match_ordered_native_payload_period_volume':True,
        'comparison_search_shift_trim_or_gain_conversion':False,
        'wrong_payload_length_period_volume_rejected':True,'timing':timing,
        'original_onset':onset(args.pcm_reference or args.original/'original.wav'),
        'native_onset':onset(args.native_report.parent/'native.wav'),
        'whole_flight_sound_acceptance':False}
    if args.consumed_samples:
        result['consumed_startup'] = assess_consumption(args.consumed_samples, args.original,
            args.pcm_reference, original)
        result['startup_dispatch'] = assess_startup_dispatch(args.consumed_samples,
            validation['resolved_voice_layout'], original, result['consumed_startup'], native)
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(f"Every {sum(map(len,original))} original startup music request matches native bytes/order/period/volume; onset/timing differences retained")


if __name__=='__main__':
    main()
