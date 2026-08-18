# Test MQTT over WebSockets

import paho.mqtt.client as mqtt

client = mqtt.Client(
    client_id="client1",
    transport="websockets",
    callback_api_version=1
)

client.connect("127.0.0.1", 9001)
client.publish("test/topic", "hello over websockets")
client.disconnect()