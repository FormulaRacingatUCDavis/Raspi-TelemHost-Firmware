#include "observer.h"


namespace frucd::daq
{   
    
    // need to finish these two
    void DashboardObserver::feHandler(
        const dbcppp::IMessage& msg, 
        const dbcppp::INetwork& spec, 
        const struct can_frame& frame)
    {
        std::unordered_map<std::string, std::pair<const dbcppp::ISignal*, double>> sig_map;
        for (const dbcppp::ISignal& sig : msg.Signals())
        {
            double phys = sig.RawToPhys(sig.Decode(frame.data));
            sig_map[sig.Name()] = std::make_pair(&sig, phys);
        }

        switch (msg.Id()) {
            case 0x766:
                if (auto state = GetStringEncoding(*(sig_map["State"].first), static_cast<int64_t>(sig_map["State"].second));
                    state.has_value())
                {
                    Variables::setvehiclestate(state.value(), sig_map["State"].second);
                }
                break;
            
            case 0x381:
                Variables::setsoc(sig_map["SOC"].second);
                Variables::setpacktemp(sig_map["HI_TEMP"].second);
                break;

            case 0x387:
                if (sig_map["PRECHARGE"].second == 0) {
                    Variables::setshutdowncircuit("PRECHARGE");
                } else if (sig_map["AIR_NEG"].second == 0) {
                    Variables::setshutdowncircuit("AIR NEG");
                } else if (sig_map["AIR_POS"].second == 0) {
                    Variables::setshutdowncircuit("AIR POS");
                } else if (sig_map["BMS_OK"].second == 0) {
                    Variables::setshutdowncircuit("BMS OK");
                } else if (sig_map["IMD_OK"].second == 0) {
                    Variables::setshutdowncircuit("IMD OK");
                } else if (sig_map["SHUTDOWN_FINAL"].second == 0) {
                    Variables::setshutdowncircuit("SHUTDOWN FINAL");
                } else {
                    Variables::setshutdowncircuit("NO SHUTDOWN");
                } 
                break;
        }
    }

    void DashboardObserver::mcHandler(
        const dbcppp::IMessage& msg, 
        const dbcppp::INetwork& spec, 
        const struct can_frame& frame)
    {
        std::unordered_map<std::string, double> sig_map;
        for (const dbcppp::ISignal& sig : msg.Signals())
        {
            double phys = sig.RawToPhys(sig.Decode(frame.data));
            sig_map[sig.Name()] = phys;
        }

        switch (msg.Id()) {
            case 0x0A0:
                Variables::setmctemp(sig_map["INV_Module_A_Temp"], 
                                     sig_map["INV_Module_B_Temp"],
                                     sig_map["INV_Module_C_Temp"]);
                break;
            
            case 0x0A2:
                Variables::setmotortemp(sig_map["INV_Motor_Temp"]);
                break;
            
            case 0x0A9:
                Variables::setglvv(sig_map["INV_Ref_Voltage_12_0"]);
                break;

            case 0x0AB:
                if ((sig_map["INV_Post_Fault_Lo"] != 0.0) && (sig_map["INV_Post_Fault_Hi"] != 0.0))
                {
                    // make sure this gets right value
                    uint32_t faultid = (uint32_t) sig_map["INV_Post_Fault_Lo"] 
                                        | ((uint32_t) sig_map["INV_Post_Fault_Hi"] << 16);
                    Variables::setmcfault(faultid, true);
                } else if ((sig_map["INV_Run_Fault_Lo"] != 0.0) && (sig_map["INV_Run_Fault_Hi"] != 0.0)) {
                    // make sure this gets right value
                    uint32_t faultid = (uint32_t) sig_map["INV_Run_Fault_Lo"] 
                                        | ((uint32_t) sig_map["INV_Run_Fault_Hi"] << 16);
                    Variables::setmcfault(faultid, false);
                }
                break;

            case 0x0A5:
                Variables::setmph(std::abs(sig_map["INV_MOTOR_SPEED"]) * 0.016349); // = rpm * (60 * pi * Tire Diameter) / (Final Drive Ratio * 63360)
                break; 
        }
    }
}