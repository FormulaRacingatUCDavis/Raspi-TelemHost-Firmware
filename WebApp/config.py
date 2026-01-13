import platform

if platform.system() == 'Linux':
    CANBuses = dict(
        tcan = dict(
            channel = 'vcan1',
            interface = 'socketcan'
        ),
        pcan = dict(
            channel = 'vcan0',
            interface = 'socketcan'
        )
    )
else:
    CANBuses = dict(
        tcan = dict(
            channel = 'vcan1',
            interface = 'virtual'
        ),
        pcan = dict(
            channel = 'vcan0',
            interface = 'virtual'
        )
    )