"""Decode retained uncompressed UAE CIA-A and Paula savestate chunks."""

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


