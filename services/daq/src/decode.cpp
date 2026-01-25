#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "decode.h"

namespace telem
{
    nlohmann::json decode_to_json(const telem::Capture& cap)
    {
        uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG) ? (cap.frame.can_id & CAN_EFF_MASK) : (cap.frame.can_id & CAN_SFF_MASK);
        nlohmann::json j;

        j["id"] = id;

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(cap.timestamp.time_since_epoch()).count();
        j["timestamp"] = ms;
        
        switch (id)
        {
            case 0x766:
            {
                struct fe12_db_vehicle_state_t msg;
                fe12_db_vehicle_state_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["dashboard_hv_requested"]   = fe12_db_vehicle_state_dashboard_hv_requested_decode(msg.dashboard_hv_requested);
                j["dashboard_throttle1_level"] = fe12_db_vehicle_state_dashboard_throttle1_level_decode(msg.dashboard_throttle1_level);
                j["dashboard_throttle2_level"] = fe12_db_vehicle_state_dashboard_throttle2_level_decode(msg.dashboard_throttle2_level);
                j["dashboard_brake_level"]    = fe12_db_vehicle_state_dashboard_brake_level_decode(msg.dashboard_brake_level);
                j["dashboard_vcu_ticks"]      = fe12_db_vehicle_state_dashboard_vcu_ticks_decode(msg.dashboard_vcu_ticks);

                uint8_t state_key = fe12_db_vehicle_state_dashboard_state_decode(msg.dashboard_state);
                j["dashboard_state"] = (VEHICLE_STATE.find(state_key) != VEHICLE_STATE.end()) 
                                    ? VEHICLE_STATE.at(state_key) 
                                    : "YO WTF?";

                break;
            }
            case 0xC0:
            {
                struct fe12_db_torque_request_t msg;
                fe12_db_torque_request_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["dashboard_torque"] = fe12_db_torque_request_dashboard_torque_decode(msg.dashboard_torque);
                j["dashboard_speed"] = fe12_db_torque_request_dashboard_speed_decode(msg.dashboard_speed);
                j["dashboard_direction"] = fe12_db_torque_request_dashboard_direction_decode(msg.dashboard_direction);
                j["dashboard_inverter_enable"] = fe12_db_torque_request_dashboard_inverter_enable_decode(msg.dashboard_inverter_enable);
                j["dashboard_discharge_enable"] = fe12_db_torque_request_dashboard_discharge_enable_decode(msg.dashboard_discharge_enable);
                j["dashboard_speed_mode_enable"] = fe12_db_torque_request_dashboard_speed_mode_enable_decode(msg.dashboard_speed_mode_enable);
                j["dashboard_torque_limit"] = fe12_db_torque_request_dashboard_torque_limit_decode(msg.dashboard_torque_limit);

                break;
            }
            case 0xA0:
            {
                struct cm200_db_m160_temperature_set_1_t msg;
                cm200_db_m160_temperature_set_1_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["inv_module_a_temp"] = cm200_db_m160_temperature_set_1_inv_module_a_temp_decode(msg.inv_module_a_temp);
                j["inv_module_b_temp"] = cm200_db_m160_temperature_set_1_inv_module_b_temp_decode(msg.inv_module_b_temp);
                j["inv_module_c_temp"] = cm200_db_m160_temperature_set_1_inv_module_c_temp_decode(msg.inv_module_c_temp);
                j["inv_gate_driver_board_temp"] = cm200_db_m160_temperature_set_1_inv_gate_driver_board_temp_decode(msg.inv_gate_driver_board_temp);

                break;
            }
        }

        return j;
    }
}