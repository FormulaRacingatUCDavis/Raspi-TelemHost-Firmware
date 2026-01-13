import time
import csv
import threading
import queue
import os
from datetime import datetime
from ingestion import CANReader

class CANEngine:
    """
    Manages data from both CAN buses
    """
    def __init__(self):
        self.pcan_reader = CANReader('pcan')
        self.tcan_reader = CANReader('tcan')

        self.stop = threading.Event()

        self.latest_pcan = {'id': None, 'data': None, 'timestamp': None}
        self.latest_tcan = {'id': None, 'data': None, 'timestamp': None}

        self._log_pcan = None
        self._log_tcan = None

    def _write_can(self, reader: CANReader, q):
        print(f"[LOG THREAD] Started for {reader.name}")

        with open(reader.log_path, 'w', newline='') as f:
            writer = csv.DictWriter(
                f,
                fieldnames=['ID', 'D0','D1','D2','D3','D4','D5','D6','D7','Timestamp']
            )
            writer.writeheader()

            start_time = time.monotonic()
            count = 0

            while not self.stop.is_set():
                try:
                    msg = q.get(timeout=0.5)
                except queue.Empty:
                    continue

                data = list(msg.data)
                ts = time.monotonic() - start_time

                latest = self.latest_pcan if reader.name == "pcan" else self.latest_tcan
                latest['id'] = msg.arbitration_id
                latest['data'] = data
                latest['timestamp'] = ts

                row = {
                    'ID': hex(msg.arbitration_id)[2:].upper(),
                    'Timestamp': int(ts * 1000)
                }
                for i in range(8):
                    row[f"D{i}"] = data[i] if i < len(data) else 0

                writer.writerow(row)
                count += 1

                if count >= 250:
                    f.flush()
                    count = 0

    def start_log(self):
        print("[ENGINE] Starting CAN capture")

        if self._log_pcan and self._log_pcan.is_alive():
            print("[ENGINE] Already running")
            return False

        self.stop.clear()

        self.pcan_reader = CANReader('pcan')
        self.tcan_reader = CANReader('tcan')

        self.latest_pcan = {'id': None, 'data': None, 'timestamp': None}
        self.latest_tcan = {'id': None, 'data': None, 'timestamp': None}

        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
        self.pcan_reader.log_path = os.path.join(self.pcan_reader.log_dir, f"{timestamp}_pcan.csv")
        self.tcan_reader.log_path = os.path.join(self.tcan_reader.log_dir, f"{timestamp}_tcan.csv")

        self.pcan_reader.create_process()
        self.tcan_reader.create_process()

        self.pcan_reader.q_proc.start()
        self.tcan_reader.q_proc.start()

        self._log_pcan = threading.Thread(
            target=self._write_can,
            args=(self.pcan_reader, self.pcan_reader.q),
            daemon=True
        )
        self._log_tcan = threading.Thread(
            target=self._write_can,
            args=(self.tcan_reader, self.tcan_reader.q),
            daemon=True
        )

        self._log_pcan.start()
        self._log_tcan.start()

        return True

    def stop_log(self):
        print("[ENGINE] Stopping CAN capture")
        self.stop.set()

        for reader in (self.pcan_reader, self.tcan_reader):
            if reader.q_proc and reader.q_proc.is_alive():
                reader.q_stop.set()
                reader.q_proc.join(timeout=1)
                if reader.q_proc.is_alive():
                    reader.q_proc.terminate()

        if self._log_pcan and self._log_pcan.is_alive():
            self._log_pcan.join()
        if self._log_tcan and self._log_tcan.is_alive():
            self._log_tcan.join()

        self._log_pcan = None
        self._log_tcan = None