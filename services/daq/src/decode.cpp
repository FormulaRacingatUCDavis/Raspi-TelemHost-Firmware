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

        std::time_t t = std::chrono::system_clock::to_time_t(cap.timestamp);
        std::tm tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
        j["timestamp"] = ss.str();

        switch (id)
        {
            case 0x766:
            {
                struct fe12_db_vehicle_state_t msg;
                fe12_db_vehicle_state_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["dashboard_hv_requested"] = msg.dashboard_hv_requested;
                j["dashboard_throttle1_level"] = msg.dashboard_throttle1_level;
                j["dashboard_throttle2_level"] = msg.dashboard_throttle2_level;
                j["dashboard_brake_level"] = msg.dashboard_brake_level;
                j["dashboard_vcu_ticks"] = msg.dashboard_vcu_ticks;

                uint8_t state_key = fe12_db_vehicle_state_dashboard_state_decode(msg.dashboard_state);
                j["dashboard_state"] = (VEHICLE_STATE.find(state_key) != VEHICLE_STATE.end()) 
                                    ? VEHICLE_STATE.at(state_key) 
                                    : "YO WTF?";

                break;
            }
            case 0x0C0:
            {
                struct fe12_db_torque_request_t msg;
                fe12_db_torque_request_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["dashboard_torque"] = msg.dashboard_torque;
                j["dashboard_speed"] = msg.dashboard_speed;
                j["dashboard_direction"] = msg.dashboard_direction;
                j["dashboard_inverter_enable"] = msg.dashboard_inverter_enable;
                j["dashboard_discharge_enable"] = msg.dashboard_discharge_enable;
                j["dashboard_speed_mode_enable"] = msg.dashboard_speed_mode_enable;
                j["dashboard_torque_limit"] = msg.dashboard_torque_limit;

                break;
            }
        }

        return j;
    }
}