#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "decode.h"

namespace telem
{
    std::string format_timestamp(std::chrono::system_clock::time_point timestamp)
    {
        std::time_t t = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return ss.str();
    }

    nlohmann::json decode_to_json(const telem::Capture& cap)
    {
        uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG) ? (cap.frame.can_id & CAN_EFF_MASK) : (cap.frame.can_id & CAN_SFF_MASK);
        nlohmann::json j;

        switch (id)
        {
            case 0x766:
                struct fe12_db_vehicle_state_t msg;
                fe12_db_vehicle_state_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["id"] = id;
                j["dashboard_hv_requested"] = msg.dashboard_hv_requested;
                j["dashboard_throttle1_level"] = msg.dashboard_throttle1_level;
                j["dashboard_throttle2_level"] = msg.dashboard_throttle2_level;
                j["dashboard_brake_level"] = msg.dashboard_brake_level;
                j["dashboard_vcu_ticks"] = msg.dashboard_vcu_ticks;
                j["timestamp"] = format_timestamp(cap.timestamp);

                uint8_t state_key = fe12_db_vehicle_state_dashboard_state_decode(msg.dashboard_state);
                j["dashboard_state"] = (VEHICLE_STATE.find(state_key) != VEHICLE_STATE.end()) 
                                    ? VEHICLE_STATE.at(state_key) 
                                    : "YO WTF?";

                break;
        }

        return j;
    }
}