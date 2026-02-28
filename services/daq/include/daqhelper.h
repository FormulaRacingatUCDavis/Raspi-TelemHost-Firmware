#pragma once

#include <cstdint>
#include <thread>
#include <moodycamel/concurrentqueue.h>
#include <linux/can.h>
#include <net/if.h>
#include <chrono>
#include <nlohmann/json.hpp>
#include <unordered_map>

extern "C" {
    #include "fe12_db.h"
    #include "cm200_db.h"
}

namespace telem
{
    struct Capture {
        struct can_frame frame;
        std::chrono::system_clock::time_point timestamp;
    };

    inline const std::unordered_map<uint8_t, std::string> VEHICLE_STATE =
    {
        {0x0, "LV"},
        {0x1, "Precharge"},
        {0x2, "HV"},
        {0x3, "Drive"},
        {0x5, "Startup"},
        {0x81, "[FAULT] Drive request from LV"},
        {0x82, "[FAULT] Precharge timeout"},
        {0x83, "[FAULT] Brake not pressed"},
        {0x84, "[FAULT] HV disabled while driving"},
        {0x85, "[FAULT] Sensor discrepancy"},
        {0x86, "[FAULT] BSPD tripped"},
        {0x87, "[FAULT] Shutdown circuit open"},
        {0x88, "[FAULT] Uncalibrated"},
        {0x89, "[FAULT] Hard BSPD"},
        {0x8A, "[FAULT] MC fault"},
    };

    inline const std::unordered_map<uint8_t, std::string> BMS_STATE =
    {
        {0x0, "Normal"},
        {0x1, "Charge mode"},
        {0x2, "[FAULT] Pack temp over"},
        {0x4, "[FAULT] Pack temp under"},
        {0x8, "[FAULT] Cell volt over"},
        {0x10, "[FAULT] Cell volt under"},
        {0x20, "[FAULT] Open Wire - Off-board disconnect between cell and BMS IC"},
        {0x40, "[FAULT] Mismatch - Discrepancy between main ADC and redundant ADC cell voltage readings (likely due to on-board disconnect between cell and BMS IC)"},
        {0x80, "[FAULT] SPI fault"}
    };

    inline const std::unordered_map<uint32_t, std::string> MC_STATE =
    {
        {0x0, "Normal"},
        {0x1, "[POST FAULT] Hardware Gate/Desaturation Fault"},
        {0x2, "[POST FAULT] HW Over-current Fault"},
        {0x4, "[POST FAULT] Accelerator Shorted"},
        {0x8, "[POST FAULT] Accelerator Open"},
        {0x10, "[POST FAULT] Current Sensor Low"},
        {0x20B, "[POST FAULT] Current Sensor High"},
        {0x40, "[POST FAULT] Module Temperature Low"},
        {0x80, "[POST FAULT] Module Temperature High"},
        {0x100, "[POST FAULT] Control PCB Temperature Low"},
        {0x200, "[POST FAULT] Control PCB Temperature High"},
        {0x400, "[POST FAULT] Gate Drive PCB Temperature High"},
        {0x800, "[POST FAULT] Gate Drive PCB Temperature High"},
        {0x1000, "[POST FAULT] 5V Sense Voltage Low"},
        {0x2000, "[POST FAULT] 5V Sense Voltage High"},
        {0x4000, "[POST FAULT] 12V Sense Voltage Low"},
        {0x8000, "[POST FAULT] 12V Sense Voltage High"},
        {0x10000, "[POST FAULT] 2.5V Sense Voltage Low"},
        {0x20000, "[POST FAULT] 2.5V Sense Voltage High"},
        {0x40000, "[POST FAULT] 1.5V Sense Voltage Low"},
        {0x80000, "[POST FAULT] 1.5V Sense Voltage High"},
        {0x100000, "[POST FAULT] DC Bus Voltage High"},
        {0x200000, "[POST FAULT] DC Bus Voltage Low"},
        {0x400000, "[POST FAULT] Pre-charge Timeout"},
        {0x800000, "[POST FAULT] Pre-charge Voltage Failure"},
        {0x1000000, "[POST FAULT] EEPROM Checksum Invalid"},
        {0x2000000, "[POST FAULT] EEPROM Data Out of Range"},
        {0x4000000, "[POST FAULT] EEPROM Update Required"},
        {0x8000000, "[POST FAULT] Reserved"},
        {0x10000000, "[POST FAULT] Gate Driver Initialization"},
        {0x20000000, "[POST FAULT] Reserved"},
        {0x40000000, "[POST FAULT] Brake Shorted"},
        {0x80000000, "[POST FAULT] Brake Open"},
        {0x1, "[RUN FAULT] Motor Over-Speed Fault"},
        {0x2, "[RUN FAULT] Over-current Fault"},
        {0x4, "[RUN FAULT] Over-voltage Fault"},
        {0x8, "[RUN FAULT] Inverter Over-temperature Fault"},
        {0x10, "[RUN FAULT] Accelerator Input Shorted Fault"},
        {0x20, "[RUN FAULT] Accelerator Input Open Fault"},
        {0x40, "[RUN FAULT] Direction Command Fault"},
        {0x80, "[RUN FAULT] Inverter Response Time-out Fault"},
        {0x100, "[RUN FAULT] Hardware Gate/Desaturation Fault"},
        {0x200, "[RUN FAULT] Hardware Over-current Fault"},
        {0x400, "[RUN FAULT] Under-voltage Fault"},
        {0x800, "[RUN FAULT] CAN Command Message Lost Fault"},
        {0x1000, "[RUN FAULT] Motor Over-temperature Fault"},
        {0x2000, "[RUN FAULT] Reserved"},
        {0x4000, "[RUN FAULT] Reserved"},
        {0x8000, "[RUN FAULT] Reserved"},
        {0x10000, "[RUN FAULT] Brake Input Shorted Fault"},
        {0x2000, "[RUN FAULT] Brake Input Open Fault"},
        {0x40000, "[RUN FAULT] Module A Over-temperature Fault"},
        {0x80000, "[RUN FAULT] Module B Over-temperature Fault"},
        {0x100000, "[RUN FAULT] Module C Over-temperature Fault"},
        {0x200000, "[RUN FAULT] PCB Over-temperature"},
        {0x400000, "[RUN FAULT] GDB1 Over-temperature"},
        {0x800000, "[RUN FAULT] GDB2 Over-temperature"},
        {0x1000000, "[RUN FAULT] GDB3 Over-temperature"},
        {0x2000000, "[RUN FAULT] Current sensor fault"},
        {0x4000000, "[RUN FAULT] Gate Driver Over-voltage"},
        {0x8000000, "[RUN FAULT] Reserved"},
        {0x10000000, "[RUN FAULT] Hardware Over-voltage"},
        {0x20000000, "[RUN FAULT] Reserved"},
        {0x40000000, "[RUN FAULT] Resolver Fault"},
        {0x80000000, "[RUN FAULT] Reserved"}
    };

    class DAQHelper
    {
    public:
        explicit DAQHelper(const char* interface);
        ~DAQHelper();
        void queue_frame(moodycamel::ConcurrentQueue<telem::Capture> &q);

    private:
        int s;
        struct ifreq ifr;
        struct sockaddr_can addr;
        int nbytes;
        bool read_frame(telem::Capture& cap);
    };

    std::string format_timestamp(std::chrono::system_clock::time_point timestamp);
}