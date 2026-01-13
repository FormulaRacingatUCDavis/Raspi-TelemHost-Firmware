import can
import os
from config import CANBuses
from multiprocessing import Process, Queue, Event

class CANReader:
    """
    Handles CAN bus data ingestion for ONE CAN interface.
    """
    def __init__(self, bus):
        self.name = bus
        self.q = Queue()
        self.q_stop = Event()

        if bus not in CANBuses:
            raise ValueError(f"Invalid bus '{bus}'")

        root = os.path.dirname(os.path.abspath(__file__))
        chan = CANBuses[bus]['channel']

        if chan in ('vcan0', 'vcan1'):
            self.log_dir = os.path.join(root, f'TestLogs/{bus}')
        else:
            self.log_dir = os.path.join(root, f'Logs/{bus}')

        os.makedirs(self.log_dir, exist_ok=True)

        self.log_path = None
        self.q_proc = None

    def create_process(self):
        """
        Create a new CAN-reading process each time logging starts.
        """
        self.q_stop.clear()
        self.q_proc = Process(
            target=self._queue_can,
            args=(self.q, self.q_stop, self.name),
            name=f'{self.name}-reader'
        )

    @staticmethod
    def _queue_can(queue, stop, name):
        """
        Reader loop running inside the process.
        """
        bus = can.interface.Bus(
            channel=CANBuses[name]['channel'],
            interface=CANBuses[name]['interface']
        )

        try:
            while not stop.is_set():
                msg = bus.recv(timeout=0.5)
                if msg is not None:
                    queue.put(msg)
        finally:
            try:
                bus.shutdown()
            except:
                pass
