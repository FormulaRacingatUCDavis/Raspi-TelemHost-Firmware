#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>

#include "daq.h"
#include "variables.h"
#include "fe12.h"
#include "gen5_inv.h"

namespace dashboard
{
    CANManager::CANManager()
    {
        struct sockaddr_can addr;
        struct ifreq ifr;

        mCanSock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

        strncpy(ifr.ifr_name, "vcan0", IFNAMSIZ - 1);
        ioctl(mCanSock, SIOCGIFINDEX, &ifr);

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        bind(mCanSock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    }

    CANManager::~CANManager()
    {
        if (mCanSock >= 0)
        {
            close(mCanSock);
            mCanSock = -1;
        }
    }

    void CANManager::decode_can()
    {
        struct can_frame frame;
        int nbytes = read(mCanSock, &frame, sizeof(struct can_frame));
        uint32_t id = (frame.can_id & CAN_EFF_FLAG) ? (frame.can_id & CAN_EFF_MASK) : (frame.can_id & CAN_SFF_MASK);

        switch (id) {
            case 0x501: // 1281 Inputs
                struct fe12_inputs_t input_msg;
                fe12_inputs_unpack(&input_msg, frame.data, frame.can_dlc);
                Variables::setdashboardpage(
                    fe12_inputs_dashboard_display_mode_decode(input_msg.dashboard_display_mode)
                );
                Variables::setovertake(
                    fe12_inputs_dashboard_overtake_decode(input_msg.dashboard_overtake)
                );
                Variables::settorquelimit(
                    fe12_inputs_dashboard_knob1_decode(input_msg.dashboard_knob1)
                );
                Variables::setlaunchcontrol(
                    fe12_inputs_dashboard_knob2_decode(input_msg.dashboard_knob2)
                );
                break;

            case 0x766: // 1894 Vehicle_State
                struct fe12_vehicle_state_t state_msg;
                fe12_vehicle_state_unpack(&state_msg, frame.data, frame.can_dlc);
                Variables::setvehiclestate(
                    fe12_vehicle_state_dashboard_state_decode(state_msg.dashboard_state), false);
                break;

            case 0x380: // 896 BMS_STATUS
                struct fe12_bms_status_t bms_status_msg;
                fe12_bms_status_unpack(&bms_status_msg, frame.data, frame.can_dlc);
                Variables::setvehiclestate(
                    fe12_bms_status_pei_bms_status_decode(bms_status_msg.pei_bms_status), true);
                break;
            
            case 0x381: // 897 Diagnostic_BMS_Data
                struct fe12_diagnostic_bms_data_t diagnostic_bms_data_msg;
                fe12_diagnostic_bms_data_unpack(&diagnostic_bms_data_msg, frame.data, frame.can_dlc);
                Variables::setsoc(
                    fe12_diagnostic_bms_data_pei_soc_decode(diagnostic_bms_data_msg.pei_soc)
                );
                Variables::setpacktemp(
                    fe12_diagnostic_bms_data_pei_hi_temp_decode(diagnostic_bms_data_msg.pei_hi_temp)
                );
                break;

            case 0x387: // 903 Status
                struct fe12_status_t status_msg;
                fe12_status_unpack(&status_msg, frame.data, frame.can_dlc);
                // surely dont need to decode since raw and physical value should be same value by dbc file
                if (status_msg.pei_precharge == 0) {
                    Variables::setshutdowncircuit("PRECHARGE");
                } else if (status_msg.pei_air_neg == 0) {
                    Variables::setshutdowncircuit("AIR NEG");
                } else if (status_msg.pei_air_pos == 0) {
                    Variables::setshutdowncircuit("AIR POS");
                } else if (status_msg.pei_bms_ok== 0) {
                    Variables::setshutdowncircuit("BMS OK");
                } else if (status_msg.pei_imd_ok == 0) {
                    Variables::setshutdowncircuit("IMD OK");
                } else if (status_msg.pei_shutdown_final == 0) {
                    Variables::setshutdowncircuit("SHUTDOWN FINAL");
                } else {
                    Variables::setshutdowncircuit("NO SHUTDOWN");
                } 
                break;
                
            case 0x0A0: // 160 M160_Temperature_Set_1:
                struct gen5_inv_m160_temperature_set_1_t temp1_msg;
                gen5_inv_m160_temperature_set_1_unpack(&temp1_msg, frame.data, frame.can_dlc);
                Variables::setmctemp(
                    gen5_inv_m160_temperature_set_1_inv_module_a_temp_decode(temp1_msg.inv_module_a_temp),
                    gen5_inv_m160_temperature_set_1_inv_module_b_temp_decode(temp1_msg.inv_module_b_temp), 
                    gen5_inv_m160_temperature_set_1_inv_module_c_temp_decode(temp1_msg.inv_module_c_temp)
                );
                break;
            case 0x0A2: // 162 M162_Temperature_Set_3
                struct gen5_inv_m162_temperature_set_3_t temp3_msg;
                gen5_inv_m162_temperature_set_3_unpack(&temp3_msg, frame.data, frame.can_dlc);
                Variables::setmotortemp(
                    gen5_inv_m162_temperature_set_3_inv_motor_temp_decode(temp3_msg.inv_motor_temp)
                );
                break;
            case 0x0A9: // 169 M169_Internal_Voltages
                struct gen5_inv_m169_internal_voltages_t volt_msg;
                gen5_inv_m169_internal_voltages_unpack(&volt_msg, frame.data, frame.can_dlc);
                Variables::setglvv(
                    gen5_inv_m169_internal_voltages_inv_ref_voltage_12_0_decode(volt_msg.inv_ref_voltage_12_0)
                );
                break;
            case 0x0AB: { // 171 M171_Fault_Codes
                struct gen5_inv_m171_fault_codes_t fault_msg;
                gen5_inv_m171_fault_codes_unpack(&fault_msg, frame.data, frame.can_dlc);
                uint32_t post_fault_lo = gen5_inv_m171_fault_codes_inv_post_fault_lo_decode(fault_msg.inv_post_fault_lo);
                uint32_t post_fault_hi = gen5_inv_m171_fault_codes_inv_post_fault_hi_decode(fault_msg.inv_post_fault_hi);
                uint32_t run_fault_lo = gen5_inv_m171_fault_codes_inv_run_fault_lo_decode(fault_msg.inv_run_fault_lo);
                uint32_t run_fault_hi = gen5_inv_m171_fault_codes_inv_run_fault_hi_decode(fault_msg.inv_run_fault_hi);
                if ((post_fault_lo != 0.0) || (post_fault_hi != 0.0))
                {
                    // make sure this gets right value]
                    uint32_t faultid = post_fault_lo | (post_fault_hi << 16);
                    Variables::setmcfault(faultid, true);
                } else if ((run_fault_lo != 0.0) || (run_fault_hi != 0.0)) {
                    // make sure this gets right value
                    uint32_t faultid = run_fault_lo | (run_fault_hi << 16);
                    Variables::setmcfault(faultid, false);
                }
                break;
            }
            case 0x0A5: // 165 M165_Motor_Position_Info
                struct gen5_inv_m165_motor_position_info_t motor_pos_msg;
                gen5_inv_m165_motor_position_info_unpack(&motor_pos_msg, frame.data, frame.can_dlc);
                Variables::setmph(
                    gen5_inv_m165_motor_position_info_inv_motor_speed_decode(motor_pos_msg.inv_motor_speed * 0.016349)
                ); // = rpm * (60 * pi * Tire Diameter) / (Final Drive Ratio * 63360)
                break; 
        }
    } 
}