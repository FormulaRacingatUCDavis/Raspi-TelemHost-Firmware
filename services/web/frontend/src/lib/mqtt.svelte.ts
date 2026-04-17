import mqtt, { MqttClient } from "mqtt";
import { writable } from "svelte/store";

export const client = writable<MqttClient | null>(null);
export const mqttDataNum = writable<Record<string, number>>({});
export const mqttDataStr = writable<Record<string, string>>({});

const mqttClient = mqtt.connect("ws://192.168.105.2:8080");
client.set(mqttClient);

mqttClient.on("connect", () => {
    console.log("[MQTT] Connected to broker.");

    mqttClient.subscribe("can/frame");
    mqttClient.subscribe("can/log/status");
    mqttClient.subscribe("camera/log/status");
});

mqttClient.on("message", (topic, message) => {
    let msg;
    try {
        msg = JSON.parse(message.toString());
    } catch (err) {
        console.error("[MQTT] Bad JSON:", message.toString());
        return;
    }

    if (topic === "can/log/status") {
        mqttDataStr.update((data) => ({
            ...data,
            logStatus: msg.status,
            logReq: msg.status === "on" ? "Stop Log" : "Start Log",
            timestamp: msg.timestamp,
        }));
    }
    else if (topic === "camera/log/status") {
        mqttDataStr.update((data) => ({
            ...data,
            recStatus: msg.status,
            recReq: msg.status === "on" ? "Stop Camera" : "Record Camera",
            timestamp: msg.timestamp,
        }));
    }
    else if (topic === "can/frame") {
        switch (msg.id) {
            case 0xA0: {
                mqttDataNum.update((data) => ({
                    ...data,
                    moduleTempA: msg.inv_module_a_temp,
                    moduleTempB: msg.inv_module_b_temp,
                    moduleTempC: msg.inv_module_c_temp,
                    timestamp: msg.timestamp,
                }));
                break;
            }
            case 0x766: {
                mqttDataStr.update((data) => ({
                    ...data,
                    vcuState: msg.dashboard_state,
                    timestamp: msg.timestamp,
                }));
                break;
            }
            case 0x380: {
                mqttDataStr.update((data) => ({
                    ...data,
                    bmsState: msg.pei_bms_status,
                    timestamp: msg.timestamp,
                }));
                break;
            }
            case 0xAB: {
                mqttDataStr.update((data) => ({
                    ...data,
                    mcState: msg.inv_run_fault !== "Normal" ? msg.inv_run_fault : msg.inv_post_fault,
                    timestamp: msg.timestamp,
                }));
                break;
            }
            case 0xC0: {
                mqttDataNum.update((data) => ({
                    ...data,
                    dashboard_torque: msg.dashboard_torque,
                    timestamp: msg.timestamp,
                }));
                break;
            }
            case 0xA5: {
                mqttDataNum.update((data) => ({
                    ...data,
                    inv_motor_speed: msg.inv_motor_speed,
                    timestamp: msg.timestamp,
                }));
                break;
            }
            case 0x381: {
                mqttDataNum.update((data) => ({
                    ...data,
                    pei_soc: msg.pei_soc,
                    timestamp: msg.timestamp,
                }));
                break;
            }
        }
    }
});
