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
            std::cout << phys << "\n";
            sig_map[sig.Name()] = std::make_pair(&sig, phys);
        }

        switch (msg.Id()) {
            case 0x766:
                auto [sig, stateNum] = sig_map["State"];
                if (auto state = GetStringEncoding(*sig, static_cast<int64_t>(stateNum));
                    state.has_value())
                {
                    Variables::setvehiclestate(state.value());
                }
                break;
        }
    }

    void DashboardObserver::mcHandler(
        const dbcppp::IMessage& msg, 
        const dbcppp::INetwork& spec, 
        const struct can_frame& frame)
    {
        std::cout << msg.Id() << "\n";

    }
}