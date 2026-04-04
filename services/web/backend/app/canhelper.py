import shutil
import cantools
import os
import csv
import sqlite3
import psycopg2
from psycopg2 import sql

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

        table_name = "log" + log[:-4]
        with self.conn.cursor() as cursor:
            column_definitions = ", ".join([f"{sig} TEXT" for sig in sorted(signals)])
            query_create_table = ("""
                CREATE TABLE IF NOT EXISTS {table} (
                    time TIMESTAMPTZ NOT NULL,
                    source TEXT,
                    id INTEGER,
                    message TEXT,
                    {signals}
                    );""").format(table=table_name, signals=column_definitions)
            # query_create_sensordata_table = "CREATE TABLE IF NOT EXISTS " + log + " (Timestamp TIMESTAMPTZ NOT NULL, Source TEXT, ID INTEGER, Message TEXT".join(sorted(signals))
            query_create_hypertable = "SELECT create_hypertable('{table}', by_range('time'));".format(table=table_name) 
            cursor.execute(query_create_table)
            cursor.execute(query_create_hypertable)
            self.conn.commit()
            print("set up table")

        with self.conn.cursor() as cur:
            compress_setup_query = ("""
                ALTER TABLE {table} SET (
                    timescaledb.compress,
                    timescaledb.compress_segmentby = 'id',
                    timescaledb.compress_orderby = 'time DESC'
                )
            """).format(table=table_name)
            cur.execute(compress_setup_query)
            self.conn.commit()
            print("enable compress")

        try:
            with self.conn.cursor() as cur:
                with open(os.path.join(self.config["paths"]["data"]["can"]["parsed"], log), 'r') as f:
                    copy_query = "COPY " + table_name + " FROM STDIN WITH CSV DELIMITER ','"
                    cur.copy_expert(copy_query, f)
                self.conn.commit()

                compress_query = ("""
                    SELECT compress_chunk(i, if_not_compressed => true) 
                    FROM show_chunks({table}) i;
                """).format(table=table_name)
                
                cur.execute(compress_query)
                self.conn.commit() 
                print("copy and compress")
        except Exception as e:
            self.conn.rollback()
            print(f"Error: {e}")

        shutil.move(path, os.path.join(self.config["paths"]["data"]["can"]["raw"], log))

    # TODO: get numerical/string data