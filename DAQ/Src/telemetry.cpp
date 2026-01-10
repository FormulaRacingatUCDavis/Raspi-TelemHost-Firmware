#include "telemetry.h"
#include "config.h"
#include "variables.h"
#include "observer.h"
#include "fe12.h"
#include "gen5_inv.h"

#include <chrono>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <numeric>
#include <filesystem>
#include <fstream>
#include <dbcppp/Network.h>
#include <dbcppp/Message.h>
#include <typeinfo>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

extern "C" {
    #include "ADS1263.h"
    #include "DEV_Config.h"
}

namespace frucd::daq
{
    static constexpr std::string_view gFields = "ID,D0,D1,D2,D3,D4,D5,D6,D7,Timestamp";
    static std::string get_formatted_time();

    TelemetryManager::TelemetryManager(const Config& cfg)
        : mConfig(cfg)
    {
    }

    TelemetryManager::~TelemetryManager()
    {
        if (mCanSock >= 0)
        {
            close(mCanSock);
            mCanSock = -1;
        }
    }

    // void TelemetryManager::register_can_observer(const CanCallback& feHandler, const CanCallback& mcHandler)
    // {
    //     mObservers.emplace_back(feHandler, mcHandler);
    // }

    // std::filesystem::path TelemetryManager::init_csv()
    // {
    //     auto path = get_csv_path(mConfig.logsDir);
    //     mCsvFile.open(path, std::ios::out | std::ios::trunc);
    //     if (!mCsvFile.is_open())
    //         throw std::runtime_error("Failed to open CSV file: " + path.string());

    //     mCsvFile << gFields << '\n';
    //     mCsvFile.flush();

    //     return path;
    // }

    void TelemetryManager::init_can()
    {
        std::ifstream feDbc(mConfig.feDbcFile); // Fe12.dbc
        std::ifstream mcDbc(mConfig.mcDbcFile); // 20240129 Gen5 CAN CB.dbc
        if (!feDbc.is_open() || !mcDbc.is_open())
            throw std::runtime_error("Failed to open DBC files from config.json");

        // mFeSpec = dbcppp::INetwork::LoadDBCFromIs(feDbc);
        // mMcSpec = dbcppp::INetwork::LoadDBCFromIs(mcDbc);
        // if (!mFeSpec.get())
        //     throw std::runtime_error("Failed to parse FRUCD DBC file.");
        // else if (!mMcSpec.get())
        //     throw std::runtime_error("Failed to parse CM200 Inverter DBC file.");

        // for (const auto& msg : mFeSpec->Messages())
        //     mFeMsgs.insert(std::make_pair(msg.Id(), &msg));
        // for (const auto& msg : mMcSpec->Messages())
        //     mMcMsgs.insert(std::make_pair(msg.Id(), &msg));

        mCanSock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (mCanSock == -1)
            throw std::runtime_error("Failed to open CAN socket!");

        struct sockaddr_can addr{};
        struct ifreq ifr{};
        strncpy(ifr.ifr_name, mConfig.canNode.c_str(), IFNAMSIZ - 1);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';

        if (ioctl(mCanSock, SIOCGIFINDEX, &ifr) < 0)
        {
            int savedErr = errno;
            close(mCanSock);
            mCanSock = -1;
            throw std::runtime_error("Failed to get CAN interface index for " + mConfig.canNode + ": " + strerror(savedErr));
        }

        timeval tv{};
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(mCanSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(mCanSock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
        {
            int savedErr = errno;
            close(mCanSock);
            mCanSock = -1;
            throw std::runtime_error("Failed to bind to CAN interface " + mConfig.canNode + ": " + strerror(savedErr));
        }

        mCanInitialized = true;
        std::cout << "Initialized CAN on " << mConfig.canNode << " (ifindex " << ifr.ifr_ifindex << ")" << std::endl;
    }

    void TelemetryManager::log_can()
    {

        // if (!mCanInitialized || mCanSock < 0)
        // {
        //     std::cerr << "log_can called but CAN not initialized!\n";
        //     return;
        // }
        // if (!mFeSpec || !mMcSpec)
        // {
        //     std::cerr << "log_can called without loaded DBC specs!\n";
        //     return;
        // }
        // can_frame frame{};
        // int numBytes = read(mCanSock, &frame, sizeof(can_frame));
        // if (numBytes < 0)
        // {
        //     if (errno != EAGAIN && errno != EWOULDBLOCK)
        //         perror("CAN read failed");
        //     return;
        // }

        // uint32_t id = (0x1FFFFFFF & frame.can_id);
        // if (auto it = mFeMsgs.find(id); it != mFeMsgs.end())
        // {
        //     auto* msg = it->second;
        //     for (auto& observer : mObservers)
        //         observer.feHandler(*msg, *mFeSpec, frame);
        // }
        // else if (auto it = mMcMsgs.find(id); it != mMcMsgs.end())
        // {
        //     auto* msg = it->second;
        //     for (auto& observer : mObservers)
        //         observer.mcHandler(*msg, *mMcSpec, frame);
        // }

        struct can_frame frame;
        int nbytes = read(mCanSock, &frame, sizeof(struct can_frame));
        if (nbytes == sizeof(struct can_frame)) {
            // The 'frame' struct is now automatically filled with 
            // the data from the incoming CAN message.
            printf("Received ID: 0x%X\n", frame.can_id);
         }
        
        uint32_t id = (frame.can_id & CAN_EFF_FLAG) ? (frame.can_id & CAN_EFF_MASK) : (frame.can_id & CAN_SFF_MASK);
        std::cout << id << std::endl;

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
            
            //
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


        // std::array<double, 8> values{};
        // for (int i = 0; i < frame.can_dlc && i < 8; ++i)
        // {
        //     values[i] = frame.data[i];
        // }
        // write_row(frame.can_id, std::move(values));
    } 

    // void TelemetryManager::write_row(int32_t id, std::array<double, 8>&& values)
    // {
    //     mCsvFile
    //         << id << ','
    //         << values[0] << ','
    //         << values[1] << ','
    //         << values[2] << ','
    //         << values[3] << ','
    //         << values[4] << ','
    //         << values[5] << ','
    //         << values[6] << ','
    //         << values[7] << ','
    //         << (get_timestamp() * 1000) << '\n';
    // }

    // int64_t TelemetryManager::get_timestamp()
    // {
    //     double fractional_seconds_since_epoch =
    //         std::chrono::duration_cast<std::chrono::duration<double>>(
    //             std::chrono::system_clock::now().time_since_epoch())
    //             .count();
    //     return (int64_t)fractional_seconds_since_epoch;
    // }

    // std::filesystem::path TelemetryManager::get_csv_path(const std::string& logsDir)
    // {
    //     std::filesystem::path csvPath = logsDir;
    //     std::filesystem::create_directories(csvPath);
    //     return (csvPath / get_formatted_time()).replace_extension(".csv");
    // }

    // std::string get_formatted_time()
    // {
    //     auto now = std::chrono::system_clock::now();
    //     auto in_time_t = std::chrono::system_clock::to_time_t(now);

    //     std::stringstream ss;
    //     ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    //     return ss.str();
    // }

    // constexpr double TelemetryManager::get_voltage(uint32_t rawReading)
    // {
    //     if (rawReading >> 31 == 0x1)
    //         return sRefVoltage * 2 - rawReading / 2147483648.0 * sRefVoltage;
    //     else
    //         return sRefVoltage * rawReading / 2147483648.0;
    // }
}