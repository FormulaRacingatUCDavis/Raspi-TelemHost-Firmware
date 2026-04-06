import shutil
import cantools
import os
import csv
import sqlite3
import psycopg2
from psycopg2 import sql
import io
from datetime import datetime, timedelta

class CANHelper:
    """
    Handles any anaylsis or retrieval relating to CAN data
    """
    def __init__(self, config):
        self.config = config
        self.frucd_db = cantools.db.load_file(config["paths"]["dbc"]["fe12"])
        self.mc_db = cantools.db.load_file(config["paths"]["dbc"]["mc"])
        # open connection to data base
        self.conn = psycopg2.connect(host="localhost", database="test_timescaledb", user="matthew", port="5432")
       
        self.can_message_list = self.frucd_db.messages + self.mc_db.messages
        self.can_message_name_to_id_dict = {can_msg.name: can_msg.frame_id for can_msg in self.can_message_list}
        self.can_message_signals_dict = {can_msg.name: [msg_sig.name for msg_sig in can_msg.signals] for can_msg in self.can_message_list} 
        for can_msg_name in list(self.can_message_name_to_id_dict):
            try:
                print(f"Creating {can_msg_name} table")
                with self.conn.cursor() as cursor:
                    column_definitions = ", ".join([f"{msg_sig} TEXT" for msg_sig in self.can_message_signals_dict[can_msg_name]])
                    query_create_table = ("""
                        CREATE TABLE IF NOT EXISTS {table} (
                            Timestamp TIMESTAMPTZ NOT NULL,
                            Source TEXT,
                            ID TEXT,
                            Message TEXT,
                            {signals}
                            );""").format(table=can_msg_name, signals=column_definitions)
                    
                    query_create_hypertable = "SELECT create_hypertable('{table}', 'timestamp', if_not_exists => TRUE);".format(table=can_msg_name) 
                    
                    cursor.execute(query_create_table)
                    cursor.execute(query_create_hypertable)
                    self.conn.commit()
                    print(f"Created {can_msg_name} hypertable")
            except Exception as e:
                self.conn.rollback()
                print(f"Error in creating table: {e}")

    def convert_to_timestamp_and_filter_rows(self, file_path, msg_name):
        with open(file_path, 'r') as f:
            msg_id = f"{int(self.can_message_name_to_id_dict[msg_name]):X}"
            msg_signals = self.can_message_signals_dict[msg_name]
            selected_columns = [""]
            file_start_time = datetime.strptime("20251010_193149", "%Y%m%d_%H%M%S")
            next(f)
            for line in f:
                parts = line.strip().split(',')              
                if not parts: 
                    continue
                if parts[2] != msg_id:
                    continue
                time = float(parts[0])
                timestamp = file_start_time + timedelta(milliseconds=time)
                parts[0] = timestamp.isoformat()
                yield (','.join(filter(None, parts))).rstrip(',') + '\n'

    class StringIteratorIO(io.TextIOBase):
        def __init__(self, iter):
            self.iter = iter
        def read(self, n=None):
            try:
                return next(self.iter)
            except StopIteration:
                return ''

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
                for dbc in [self.frucd_db, self.mc_db]:
                    try:
                        message = dbc.get_message_by_frame_id(id)
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
                            data_bytes.append(int(b))
                        except ValueError:
                            data_bytes.append(0)

                raw_data = bytes(data_bytes)
                timestamp = float(row[-1]) / 1000.0
                parsed_data = message.decode(raw_data)
                
                parsed_msgs.append((
                    timestamp,
                    message.senders[0],
                    id_str,
                    message.name,
                    parsed_data
                ))

                if id_str not in ids:
                    ids.add(id_str)
                    for signal in message.signals:
                        signals.add(signal.name)

        with open(os.path.join(self.config["paths"]["data"]["can"]["parsed"], log), 'w', newline='') as log_parsed:
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

        for can_msg_name in list(self.can_message_name_to_id_dict):
            print(f"Populating {can_msg_name} table")
            try:
                with self.conn.cursor() as cur:
                    f = self.convert_to_timestamp_and_filter_rows(os.path.join(self.config["paths"]["data"]["can"]["parsed"], log), can_msg_name)
                    copy_query = f"COPY {can_msg_name} FROM STDIN WITH (FORMAT CSV);"
                    cur.copy_expert(copy_query, self.StringIteratorIO(f))
                    self.conn.commit()
                    print(f"Finished populating {can_msg_name} table")
            except Exception as e:
                self.conn.rollback()
                print(f"Error: {e}")
                print(f"Failed to populate {can_msg_name} table")
        
        self.conn.close()

        # shutil.move(path, os.path.join(self.config["paths"]["data"]["can"]["raw"], log))

    # TODO: get numerical/string data