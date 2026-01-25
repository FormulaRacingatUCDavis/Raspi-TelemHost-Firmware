import can
import csv
import time

bus = can.interface.Bus(channel='vcan0', interface='socketcan')

ti = None
with open('test/example.csv', 'r', newline='') as raw_can:
    reader = csv.reader(raw_can)
    for row in reader:
        can_id = int(row[0], 16)
        tf = int(row[-1])

        data_bytes = []
        for b in row[1:9]:
            try:
                data_bytes.append(int(b) if b.strip() else 0)
            except ValueError:
                data_bytes.append(0)

        msg = can.Message(
            arbitration_id=can_id,
            data=bytes(data_bytes),
            is_extended_id=False
        )
        bus.send(msg)

        if ti is not None:
            time.sleep((tf - ti) / 1000)

        ti = tf