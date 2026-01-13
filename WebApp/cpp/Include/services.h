#pragma once

#include <string>
#include <threads>
#include "ingestion.h"

namespace telem
{
    class CANEngine
    {
    public:
        CANEngine();
        void log_can();

    private:
        CANReader pcan;
        CANReader tcan;
        void log_bus(const std::string& filename, CANReader bus);
    };
}