#pragma once

#include <nlohmann/json.hpp>
#include <unordered_map>

#include "ingestion.h"

extern "C" {
    #include "fe12_db.h"
    #include "cm200_db.h"
}

namespace telem
{
    inline const std::unordered_map<uint8_t, std::string> VEHICLE_STATE = {
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

    std::string format_timestamp(std::chrono::system_clock::time_point timestamp);
    nlohmann::json decode_to_json(const telem::Capture& cap);
}