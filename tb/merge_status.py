from pathlib import Path
import heapq
import sys
import time


def events(stream):
    time = 0
    for line in stream:
        if line.startswith('#'):
            time = int(line[1:])
        elif line.strip():
            yield time, line


signals = [(1, 'valid', 'check_valid'), (1, 'check_pass', 'check_pass'),
           (1, 'failed', 'test_failed'), (1, 'done', 'test_done'),
           (1, 'pass', 'test_pass'), (32, 'id', 'check_id'),
           (32, 'line', 'source_line'), (64, 'actual', 'actual'),
           (64, 'expected', 'expected')]

for status in Path(sys.argv[1]).rglob('*.vcd.status'):
    wave = Path(str(status).removesuffix('.status'))
    temporary = wave.with_suffix('.vcd.tmp')
    with wave.open() as source, status.open() as checks, temporary.open('w') as output:
        for line in source:
            if '$enddefinitions' in line:
                break
            output.write(line)
        output.write('$scope module TEST_STATUS $end\n')
        for width, identifier, name in signals:
            output.write(f'$var wire {width} status_{identifier} {name} $end\n')
        output.write('$upscope $end\n$enddefinitions $end\n')
        previous_time = None
        for time, line in heapq.merge(events(source), events(checks), key=lambda event: event[0]):
            if time != previous_time:
                output.write(f'#{time}\n')
                previous_time = time
            output.write(line)
    # OneDrive or an open Windows viewer can briefly lock the destination.
    for attempt in range(10):
        try:
            temporary.replace(wave)
            break
        except PermissionError:
            if attempt == 9:
                raise
            time.sleep(0.2)
    status.unlink()
