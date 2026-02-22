#include "canhelper.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can/raw.h>
#include <iostream>
#include <thread>
#include <ios>

namespace telem
{
    CANHelper::CANHelper(const char* interface) : s(socket(PF_CAN, SOCK_RAW, CAN_RAW))
    {
        strcpy(ifr.ifr_name, interface);
        if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
        {
            perror("ioctl SIOCGIFINDEX");
            exit(1);
        }

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind");
            exit(1);
        }
    }

    CANHelper::~CANHelper()
    {
        close(s);
    }

    bool CANHelper::read_frame(telem::Capture& cap)
    {
        int nbytes = read(s, &cap.frame, sizeof(struct can_frame));
        cap.timestamp = std::chrono::system_clock::now();
        return nbytes == sizeof(struct can_frame);
    }

    void CANHelper::queue_frame(moodycamel::ConcurrentQueue<telem::Capture> &q)
    {
        telem::Capture frame;

        while (true)
        {
            if (read_frame(frame))
            {
                q.enqueue(frame);
            }
        }
    }

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
            case 0xA5:
            {
                struct cm200_db_m165_motor_position_info_t msg;
                cm200_db_m165_motor_position_info_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["inv_motor_angle_electrical"] = cm200_db_m165_motor_position_info_inv_motor_angle_electrical_decode(msg.inv_motor_angle_electrical);
                j["inv_motor_speed"] = cm200_db_m165_motor_position_info_inv_motor_speed_decode(msg.inv_motor_speed);
                j["inv_electrical_output_frequency"] = cm200_db_m165_motor_position_info_inv_electrical_output_frequency_decode(msg.inv_electrical_output_frequency);
                j["inv_delta_resolver_filtered"] = cm200_db_m165_motor_position_info_inv_delta_resolver_filtered_decode(msg.inv_delta_resolver_filtered);

                break;
            }
            case 0x388:
            {
                struct fe12_db_current_t msg;
                fe12_db_current_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["pei_current"] = fe12_db_current_pei_current_decode(msg.pei_current);

                break;
            }
            case 0x381:
            {
                struct fe12_db_diagnostic_bms_data_t msg;
                fe12_db_diagnostic_bms_data_unpack(&msg, cap.frame.data, cap.frame.can_dlc);

                j["pei_hi_temp"] = fe12_db_diagnostic_bms_data_pei_hi_temp_decode(msg.pei_hi_temp);
                j["pei_soc"] = fe12_db_diagnostic_bms_data_pei_soc_decode(msg.pei_soc);
                j["pei_pack_voltage"] = fe12_db_diagnostic_bms_data_pei_pack_voltage_decode(msg.pei_pack_voltage);

                break;
            }
        }

        return j;
    }
}