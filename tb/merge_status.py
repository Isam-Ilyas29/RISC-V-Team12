from pathlib import Path
import sys

for status in Path(sys.argv[1]).glob('*.vcd.status'):
    wave = Path(str(status).removesuffix('.status'))
    text = wave.read_text()
    header, body = text.split('$enddefinitions $end', 1)
    signals = [(1, 'valid', 'check_valid'), (1, 'check_pass', 'check_pass'),
               (1, 'failed', 'test_failed'), (1, 'done', 'test_done'),
               (1, 'pass', 'test_pass'), (32, 'id', 'check_id'), (32, 'line', 'source_line'),
               (64, 'actual', 'actual'), (64, 'expected', 'expected')]
    header += '\n$scope module TEST_STATUS $end\n'
    for width, identifier, name in signals:
        header += f'$var wire {width} status_{identifier} {name} $end\n'
    header += '$upscope $end\n$enddefinitions $end\n'
    events = {}
    for source in (body, status.read_text()):
        time = 0
        for line in source.splitlines():
            if line.startswith('#'):
                time = int(line[1:])
            elif line.strip():
                events.setdefault(time, []).append(line)
    with wave.open('w') as output:
        output.write(header)
        for time, lines in sorted(events.items()):
            output.write(f'#{time}\n' + '\n'.join(lines) + '\n')
    status.unlink()
