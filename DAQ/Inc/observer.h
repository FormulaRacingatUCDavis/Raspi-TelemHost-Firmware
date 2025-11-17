#pragma once

#include "telemetry.h"
#include "variables.h"

#include <string>
#include <unordered_map>
#include <linux/can.h>
#include <dbcppp/ValueTable.h>


namespace frucd::daq
{
    class DashboardObserver
    {
    public:
        // creates a observer and its functions for the code already in telemetry.cpp
        DashboardObserver(frucd::daq::TelemetryManager& telemetryManager)
        {
            telemetryManager.register_can_observer( 
            [this](const dbcppp::IMessage& msg, const dbcppp::INetwork& spec, const struct can_frame& frame) { 
                this->feHandler(msg, spec, frame); }, 
            [this](const dbcppp::IMessage& msg, const dbcppp::INetwork& spec, const struct can_frame& frame) { 
                this->mcHandler(msg, spec, frame); } );
        }
    
    private: 
        // straight up took this from https://github.com/FormulaRacingatUCDavis/FE13FW-Display/blob/main/Source/utils.cpp
        // didnt test yet but surely just gets the string from a physical value from the can using the dbc right
        std::optional<std::string> GetStringEncoding(const dbcppp::ISignal& sig, int64_t value)
        {
            auto res = std::find_if(
                sig.ValueEncodingDescriptions().begin(),
                sig.ValueEncodingDescriptions().end(),
                [value](const dbcppp::IValueEncodingDescription& ved) { return ved.Value() == value; });
            return (res != sig.ValueEncodingDescriptions().end()) ? std::optional(res->Description()) : std::nullopt;
        }

        void feHandler(const dbcppp::IMessage& msg, const dbcppp::INetwork& spec, const struct can_frame& frame);

        void mcHandler(const dbcppp::IMessage& msg, const dbcppp::INetwork& spec, const struct can_frame& frame);
    };
}