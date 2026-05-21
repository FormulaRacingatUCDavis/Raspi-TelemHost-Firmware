import subprocess


class BTHeadsetManager:
    def __init__(self, mac_addrs):
        self.headset_rssis = dict()
        self.conn_headset_mac = None

        for mac in mac_addrs:
            self.headset_rssis[mac] = -121

    def get_headset_rssis(self):
        for mac in self.headset_rssis:
            self.headset_rssis[mac] = -121

        bt_cmd = "bluetoothctl --timeout 5 scan on | grep 'RSSI'"
        scanned_rssis = subprocess.run(bt_cmd,
                                       capture_output=True,
                                       shell=True,
                                       text=True).stdout
        scanned_rssis = scanned_rssis.splitlines()
        for line in scanned_rssis:
            line_split = line.split()
            if line_split[2] in self.headset_rssis:
                self.headset_rssis[line_split[2]] = int(line_split[4])

        '''[CHG] Device C5:1A:DC:A1:63:08 RSSI: -69
        [CHG] Device F0:A9:82:1D:9F:41 RSSI: -68
        [CHG] Device C7:AB:D4:42:8A:B4 RSSI: -76'''

        if self.conn_headset_mac is not None:
            bt_cmd = f"hcitool rssi {self.conn_headset_mac}"
            conn_headset_rssi = subprocess.run(bt_cmd,
                                               capture_output=True,
                                               shell=True,
                                               text=True).stdout
            conn_headset_rssi = int(conn_headset_rssi.split()[4])
            # RSSI return value: -6

            self.headset_rssis[self.conn_headset_mac] = conn_headset_rssi

    def connect_device(self, mac_addr):
        # TODO: ADD FAILED COMMAND EXCEPTION
        subprocess.run(f"bluetoothctl connect {mac_addr}")

    def disconnect_device(self, mac_addr):
        # TODO: ADD FAILED COMMAND EXCEPTION
        subprocess.run(f"bluetoothctl disconnect {mac_addr}")

    def connect_to_closest(self):
        self.get_headset_rssis()

        closest_headset = max(self.headset_rssis, key=self.headset_rssis.get)

        if self.conn_headset_mac is not None:
            self.disconnect_device(self.conn_headset_mac)

        if self.headset_rssis[closest_headset] != -121:
            self.connect_device(closest_headset)

    def bt_headset_man(self):
        while True:
            self.connect_to_closest()


if __name__ == "__main__":
    headset_bt_macs = [
        "F2:1A:9D:1B:B2:5D",
        "32:35:EA:4D:2C:46",
        "59:E7:35:19:28:8A"
    ]

    bt_man = BTHeadsetManager(headset_bt_macs)
    bt_man.bt_headset_man()
