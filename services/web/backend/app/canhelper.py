import shutil
import cantools
import os
import csv

class CANHelper:
    """
    Handles any analysis or retrieval relating to CAN data
    """
    def __init__(self, config):
        self.config = config

        self.dbs = []

        for dbc_path in config["paths"]["dbc"].values():
            self.dbs.append(cantools.db.load_file(dbc_path))

    def generate_parsed(self, path: str):
        parsed_msgs = []
        ids = set()
        signals = set()
        f_ids = set()
        f_count = 0

        log = os.path.basename(path)
        print(f'[LOG PARSER] Parsing {log}...')

        with open(path, 'rb') as log_raw:
            data = log_raw.read().replace(b'\x00', b'')

        lines = data.decode('utf-8', errors='replace').splitlines()

        for line in lines:
            if not line.strip():
                continue

            row = next(csv.reader([line]))

            if len(row) < 10:
                print(f'[LOG PARSER] >> Invalid row: {row}')
                f_count += 1
                continue

            id_str = row[0].strip()

            try:
                id = int(id_str, 16)
            except ValueError:
                print(f'[LOG PARSER] >> Invalid ID {id_str}')
                f_count += 1
                continue

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
                b = b.strip()

                if b == "":
                    data_bytes.append(0)
                else:
                    try:
                        data_bytes.append(int(b, 16))
                    except ValueError:
                        data_bytes.append(0)

            raw_data = bytes(data_bytes)

            try:
                timestamp = float(row[-1].strip()) / 1000.0
            except ValueError:
                print(f'[LOG PARSER] >> Invalid timestamp: {row[-1]}')
                f_count += 1
                continue

            try:
                parsed_data = message.decode(raw_data)
            except Exception as e:
                print(f'[LOG PARSER] >> Failed to decode ID {id_str} at timestamp {row[-1]}: {e}')
                f_count += 1
                continue

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

        parsed_path = os.path.join(
            self.config["paths"]["data"]["can"]["parsed"],
            log
        )

        with open(parsed_path, 'w', newline='') as log_parsed:
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

        print(f'[LOG PARSER] >> Total parsed messages: {len(parsed_msgs)}')
        print(f'[LOG PARSER] >> Total failed rows: {f_count}')
        print(f'[LOG PARSER] >> Output: {parsed_path}')

        if f_ids:
            print(f'[LOG PARSER] >> Unrecognized IDs: {[hex(x) for x in sorted(f_ids)]}')

        shutil.move(
            path,
            os.path.join(
                self.config["paths"]["data"]["can"]["raw"],
                log
            )
        )

    def get_messages(self, dbc_path):
        db = cantools.db.load_file(dbc_path)
        dbc_name = os.path.basename(dbc_path)

        messages = {}

        for message in db.messages:
            sender = message.senders[0].lower() if message.senders else "unknown"

            if sender not in messages:
                messages[sender] = {}

            messages[sender][message.name.lower()] = {
                "id": message.frame_id,
                "signals": [signal.name.lower() for signal in message.signals]
            }

        return {dbc_name: messages}