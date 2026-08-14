import shutil
import cantools
import os
import csv
from pathlib import Path

class CANHelper:
    """
    Handles any analysis or retrieval relating to CAN data
    """
    def __init__(self, config):
        self.config = config

        dbc_dir = Path(
            os.path.abspath(
                os.path.join(
                    os.path.dirname(__file__),
                    "..",
                    "..",
                    "..",
                    "..",
                    "common",
                    "dbc"
                )
            )
        )

        self.dbs = []

        for dbc_path in dbc_dir.glob("*.dbc"):
            print(f'[CAN HELPER] Loading DBC: {dbc_path.name}')
            self.dbs.append(cantools.db.load_file(dbc_path))

    def generate_parsed(self, path: str):
        parsed_msgs = []
        ids = set()
        signals = set()
        f_ids = set()
        f_count = 0

        log = os.path.basename(path)
        print(f'[LOG PARSER] Parsing {log}...')

        with open(path, 'r', newline='') as log_raw:
            reader = csv.reader(log_raw)

            for row in reader:
                id_str = row[0]
                id = int(id_str, 16)
                message = None

                for db in self.dbs:
                    try:
                        message = db.get_message_by_frame_id(id)
                        break
                    except KeyError:
                        continue

                if message is None:
                    print(f'[LOG PARSER] >> Unrecognized ID {id} at timestamp: {row[-1]}')
                    f_ids.add(id)
                    f_count += 1
                    continue

                data_bytes = []

                for b in row[1:9]:
                    if b.strip() == "":
                        data_bytes.append(0)
                    else:
                        try:
                            data_bytes.append(int(b, 16))
                        except ValueError:
                            data_bytes.append(0)

                raw_data = bytes(data_bytes)
                timestamp = float(row[-1]) / 1000.0
                parsed_data = message.decode(raw_data)

                parsed_msgs.append((
                    timestamp,
                    message.senders[0] if message.senders else "Unknown",
                    id_str,
                    message.name,
                    parsed_data
                ))

                if id_str not in ids:
                    ids.add(id_str)

                    for signal in message.signals:
                        signals.add(signal.name)

        with open(
            os.path.join(
                self.config["paths"]["data"]["can"]["parsed"],
                log
            ),
            'w',
            newline=''
        ) as log_parsed:

            signals_list = sorted(signals)
            header = ['Timestamp [s]', 'Source', 'ID', 'Message'] + signals_list

            writer = csv.writer(log_parsed)
            writer.writerow(header)

            for timestamp, source, id_str, msg_name, decoded in parsed_msgs:
                row_data = [timestamp, source, id_str, msg_name]

                for sig in signals_list:
                    value = decoded.get(sig, "")
                    row_data.append(value)

                writer.writerow(row_data)

            print(f'[LOG PARSER] >> Total failed rows: {f_count}')

        shutil.move(
            path,
            os.path.join(
                self.config["paths"]["data"]["can"]["raw"],
                log
            )
        )

    def get_messages(self, dbc_path):
        db = cantools.db.load_file(dbc_path)

        messages = {}

        for message in db.messages:
            sender = message.senders[0] if message.senders else "Unknown"

            if sender not in messages:
                messages[sender] = {}

            messages[sender][message.name] = [
                signal.name for signal in message.signals
            ]

        return messages