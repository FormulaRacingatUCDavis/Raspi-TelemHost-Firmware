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
        {0x1, "PRECHARGE"},
        {0x2, "HV ENABLED"},
        {0x3, "DRIVE"},
        {0x5, "STARTUP"},
        {0x81, "DRV FRM LV"},
        {0x82, "PRE TM OUT"},
        {0x83, "BR NOT PRS"},
        {0x84, "HV OFF DRV"},
        {0x85, "SNSR DSCRP"},
        {0x86, "BSPD TRIPD"},
        {0x87, "SHTDWN OPN"},
        {0x88, "UNCALIBRTD"},
        {0x89, "HARD BSPD"},
        {0x8A, "MC FAULT"},
    };

    class CANHelper
    {
    public:
        explicit CANHelper(const char* interface);
        ~CANHelper();
        void queue_frame(moodycamel::ConcurrentQueue<telem::Capture> &q);

    private:
        int s;
        struct ifreq ifr;
        struct sockaddr_can addr;
        int nbytes;
        bool read_frame(telem::Capture& cap);
    };

    std::string format_timestamp(std::chrono::system_clock::time_point timestamp);
    nlohmann::json decode_to_json(const telem::Capture& cap);
}