#include <mosquitto.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <atomic>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "daqhelper.h"

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

std::atomic<bool> log_en{true};

int main()
{
    nlohmann::json cfg;
    std::ifstream cfg_file("resources/config.json");
    cfg_file >> cfg;

    std::string PCAN_IFACE = cfg["can"]["pcan"];
    std::string TCAN_IFACE = cfg["can"]["tcan"];
    std::filesystem::path INTAKE_DIR = cfg["paths"]["data"]["can"]["intake"];
    std::filesystem::path PROCESS_DIR = cfg["paths"]["data"]["can"]["process"];

    cfg_file.close();

    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("can-publisher", true, nullptr);
    int rc = mosquitto_connect(mosq, "localhost", 1883, 60);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        std::cerr << "MQTT connect failed: " << mosquitto_strerror(rc) << std::endl;
        return 1;
    }

    mosquitto_message_callback_set(mosq, on_message);
    mosquitto_subscribe(mosq, nullptr, "can/log/control", 0);
    mosquitto_loop_start(mosq);

    moodycamel::ConcurrentQueue<telem::Capture> q(8192);
    telem::DAQHelper pcan(PCAN_IFACE.c_str());
    telem::DAQHelper tcan(TCAN_IFACE.c_str());
    std::thread pcan_thread(&telem::DAQHelper::queue_frame, &pcan, std::ref(q));
    std::thread tcan_thread(&telem::DAQHelper::queue_frame, &tcan, std::ref(q));

    std::filesystem::create_directories(INTAKE_DIR);
    std::filesystem::create_directories(PROCESS_DIR);
    std::ofstream log;
    auto start = std::chrono::system_clock::now();
    std::string filename;
    std::string log_status = "{\"status\":\"off\"}";
    uint8_t log_count = 0;
    mosquitto_publish(mosq, nullptr, "can/log/status", log_status.length(), log_status.c_str(), 1, true);

    std::cout << "[DAQ] Initialized MQTT broker and CAN socket threads" << std::endl;

    telem::Capture cap;
    while (true)
    {
        if (log_status == "{\"status\":\"off\"}" && log_en.load())
        {
            start = std::chrono::system_clock::now();
            auto t = std::chrono::system_clock::to_time_t(start);
            std::ostringstream datetime;
            datetime << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S");
            filename = "FE13CAN_" + datetime.str() + ".csv";

            log.open(INTAKE_DIR / filename);
            log_status = "{\"status\":\"on\"}";
            mosquitto_publish(mosq, nullptr, "can/log/status", log_status.length(), log_status.c_str(), 1, true);
            std::cout << "[DAQ] Started new log: " << filename << std::endl;
        }
        else if (log_status == "{\"status\":\"on\"}" && !log_en.load())
        {
            log.flush();
            log.close();
            log_status = "{\"status\":\"off\"}";
            mosquitto_publish(mosq, nullptr, "can/log/status", log_status.length(), log_status.c_str(), 1, true);
            std::cout << "[DAQ] Closed log: " << filename << std::endl;
            std::filesystem::path src = INTAKE_DIR / filename;
            std::filesystem::path dst = PROCESS_DIR / filename;
            if (std::filesystem::exists(dst)) std::filesystem::remove(dst);
            std::filesystem::rename(src, dst);
        }

        if (q.try_dequeue(cap))
        {
            uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG) ? (cap.frame.can_id & CAN_EFF_MASK) : (cap.frame.can_id & CAN_SFF_MASK);
            nlohmann::json j;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(cap.timestamp.time_since_epoch()).count();

            switch (id)
            {
                case 0x766:
                {
                    struct fe13_db_vehicle_state_t msg;
                    fe13_db_vehicle_state_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    uint8_t vcu_state_code = fe13_db_vehicle_state_dashboard_state_decode(msg.dashboard_state);
                    j["dashboard_state"] = (telem::VEHICLE_STATE.find(vcu_state_code) != telem::VEHICLE_STATE.end()) ? telem::VEHICLE_STATE.at(vcu_state_code) : "YO WTF?";
                    j["timestamp"] = ms;

                    break;
                }
                case 0xC0:
                {
                    struct fe13_db_torque_request_t msg;
                    fe13_db_torque_request_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["dashboard_torque"] = fe13_db_torque_request_dashboard_torque_decode(msg.dashboard_torque);
                    j["timestamp"] = ms;

                    break;
                }
                case 0xA0:
                {
                    struct cm200_db_m160_temperature_set_1_t msg;
                    cm200_db_m160_temperature_set_1_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["inv_module_a_temp"] = cm200_db_m160_temperature_set_1_inv_module_a_temp_decode(msg.inv_module_a_temp);
                    j["inv_module_b_temp"] = cm200_db_m160_temperature_set_1_inv_module_b_temp_decode(msg.inv_module_b_temp);
                    j["inv_module_c_temp"] = cm200_db_m160_temperature_set_1_inv_module_c_temp_decode(msg.inv_module_c_temp);
                    j["timestamp"] = ms;

                    break;
                }
                case 0xA5:
                {
                    struct cm200_db_m165_motor_position_info_t msg;
                    cm200_db_m165_motor_position_info_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["inv_motor_speed"] = cm200_db_m165_motor_position_info_inv_motor_speed_decode(msg.inv_motor_speed);
                    j["timestamp"] = ms;

                    break;
                }
                case 0x388:
                {
                    struct fe13_db_current_t msg;
                    fe13_db_current_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["pei_current"] = fe13_db_current_pei_current_decode(msg.pei_current);
                    j["timestamp"] = ms;

                    break;
                }
                case 0x380:
                {
                    struct fe13_db_bms_status_t msg;
                    fe13_db_bms_status_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    uint8_t bms_state_code = fe13_db_bms_status_pei_bms_status_decode(msg.pei_bms_status);
                    j["pei_bms_status"] = (telem::BMS_STATE.find(bms_state_code) != telem::BMS_STATE.end()) ? telem::BMS_STATE.at(bms_state_code) : "YO WTF?";
                    j["timestamp"] = ms;

                    break;
                }
                case 0x381:
                {
                    struct fe13_db_diagnostic_bms_data_t msg;
                    fe13_db_diagnostic_bms_data_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["pei_soc"] = fe13_db_diagnostic_bms_data_pei_soc_decode(msg.pei_soc);
                    j["pei_pack_voltage"] = fe13_db_diagnostic_bms_data_pei_pack_voltage_decode(msg.pei_pack_voltage);
                    j["timestamp"] = ms;

                    break;
                }
                case 0x504:
                {
                    struct fe13_db_mcac_power_t msg;
                    fe13_db_mcac_power_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["motor_power"] = fe13_db_mcac_power_motor_power_decode(msg.motor_power);
                    j["acc_power"] = fe13_db_mcac_power_acc_power_decode(msg.acc_power);
                    j["timestamp"] = ms;

                    break;
                }
                case 0x400:
                {
                    struct fe13_db_cooling_loop_temps_t msg;
                    fe13_db_cooling_loop_temps_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["inlet_water_temp"] = fe13_db_cooling_loop_temps_telem_node_inlet_water_temp_decode(msg.telem_node_inlet_water_temp);
                    j["outlet_water_temp"] = fe13_db_cooling_loop_temps_telem_node_outlet_water_temp_decode(msg.telem_node_outlet_water_temp);
                    j["timestamp"] = ms;

                    break;
                }
                case 0xAB:
                {
                    struct cm200_db_m171_fault_codes_t msg;
                    cm200_db_m171_fault_codes_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    uint32_t post_fault_code = (static_cast<uint32_t>(msg.inv_post_fault_hi) << 16) | static_cast<uint32_t>(msg.inv_post_fault_lo);
                    uint32_t run_fault_code = (static_cast<uint32_t>(msg.inv_run_fault_hi) << 16) | static_cast<uint32_t>(msg.inv_run_fault_lo);

                    j["id"] = id;
                    j["inv_post_fault"] = (telem::MC_STATE.find(post_fault_code) != telem::MC_STATE.end()) ? telem::MC_STATE.at(post_fault_code) : "YO WTF?";
                    j["inv_run_fault"] = (telem::MC_STATE.find(run_fault_code) != telem::MC_STATE.end()) ? telem::MC_STATE.at(run_fault_code) : "YO WTF?";
                    j["timestamp"] = ms;

                    break;
                }
                case 0x384:
                {
                    struct fe13_db_bms_temps_t msg;
                    fe13_db_bms_temps_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["pei_subpack"] = fe13_db_bms_temps_pei_subpack_decode(msg.pei_subpack);
                    j["pei_group"] = fe13_db_bms_temps_pei_group_decode(msg.pei_group);
                    j["pei_temp_1"] = fe13_db_bms_temps_pei_temp_1_decode(msg.pei_temp_1);
                    j["pei_temp_2"] = fe13_db_bms_temps_pei_temp_2_decode(msg.pei_temp_2);
                    j["pei_temp_3"] = fe13_db_bms_temps_pei_temp_3_decode(msg.pei_temp_3);
                    j["timestamp"] = ms;

                    break;
                }
                case 0x480:
                {
                    struct fe13_db_lat_lon_t msg;
                    fe13_db_lat_lon_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["latitude"] = fe13_db_lat_lon_latitude_decode(msg.latitude);
                    j["longitude"] = fe13_db_lat_lon_longitude_decode(msg.longitude);
                    j["timestamp"] = ms;

                    break;
                }
                case 0x113:
                {
                    struct fe13_db_acceleration_t msg;
                    fe13_db_acceleration_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                    j["id"] = id;
                    j["accel_x"] = fe13_db_acceleration_accel_x_decode(msg.accel_x);
                    j["accel_y"] = fe13_db_acceleration_accel_y_decode(msg.accel_y);
                    j["accel_z"] = fe13_db_acceleration_accel_z_decode(msg.accel_z);
                    j["timestamp"] = ms;

                    break;
                }
            }

            if (!j.empty())
            {
                std::string payload = j.dump();
                mosquitto_publish(mosq, nullptr, "can/frame", payload.size(), payload.c_str(), 0, false);
            }

            if (log_status == "{\"status\":\"on\"}")
            {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(cap.timestamp - start).count();

                log << std::hex << std::uppercase
                    << id << ","
                    << static_cast<int>(cap.frame.data[0]) << ","
                    << static_cast<int>(cap.frame.data[1]) << ","
                    << static_cast<int>(cap.frame.data[2]) << ","
                    << static_cast<int>(cap.frame.data[3]) << ","
                    << static_cast<int>(cap.frame.data[4]) << ","
                    << static_cast<int>(cap.frame.data[5]) << ","
                    << static_cast<int>(cap.frame.data[6]) << ","
                    << static_cast<int>(cap.frame.data[7]) << ","
                    << std::dec << elapsed
                    << std::endl;

                log_count++;
                if (log_count == 50)
                    log.flush();
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
    }

    return 0;
}

void on_message(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
    std::string topic(message->topic);
    std::string payload(static_cast<char*>(message->payload), message->payloadlen);

    if (topic == "can/log/control")
    {
        if (payload == "{\"status\":\"on\"}")
            log_en = true;
        else if (payload == "{\"status\":\"off\"}")
            log_en = false;
    }
}