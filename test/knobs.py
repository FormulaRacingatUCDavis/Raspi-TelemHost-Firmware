import os
import can
import cantools
import random

TESTKNOB = 1

db = cantools.database.load_file('FE12.dbc')
bus = can.interface.Bus(channel='vcan0', interface='socketcan')

knob = 0
while knob <= 4095:
    message = db.get_message_by_name('Inputs')

    if TESTKNOB == 1:
        data = {
            'Dashboard_Knob1': knob, 
            'Dashboard_Knob2': 0,
            'Dashboard_DISPLAY_MODE': 0,
            'Dashboard_TC': 0,
            'Dashboard_MARKER': 0,
            'Dashboard_OVERTAKE': 0
            }
    elif TESTKNOB == 2:
        data = {
            'Dashboard_Knob1': 0, 
            'Dashboard_Knob2': knob,
            'Dashboard_DISPLAY_MODE': 0,
            'Dashboard_TC': 0,
            'Dashboard_MARKER': 0,
            'Dashboard_OVERTAKE': 0
            }
            
    encoded = message.encode(data)
    msg = can.Message(arbitration_id=message.frame_id, data=encoded, is_extended_id=False)
    bus.send(msg)

    knob += 1