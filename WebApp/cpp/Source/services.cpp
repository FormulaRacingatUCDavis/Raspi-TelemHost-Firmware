#include <fstream>
#include <unistd.h>
#include <cstdint>
#include <iostream>
#include "services.h"

namespace telem
{
    CANEngine::CANEngine() : pcan(telem::CANBus::PCAN), tcan(telem::CANBus::TCAN)
    {}

    void CANEngine::log_can()
    {
        log_bus("test.csv", pcan);
    }

    void CANEngine::log_bus(const std::string& filename, CANReader bus)
    {
        std::ofstream log(filename, std::ios::app);

        if (!log.is_open())
        {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        bus.start();

        mqd_t mq = pcan.open_queue();
        struct can_frame frame {};

        while (true)
        {
            if (bus.get_frame(frame, mq))
            {
                uint32_t id = frame.can_id & CAN_EFF_MASK;

                log << std::hex << id << ",";

                for (int i = 0; i < frame.can_dlc; i++)
                {
                    log << std::hex << (int)frame.data[i];
                    if (i < frame.can_dlc - 1)
                        log << ",";
                }

                log << std::endl;
            }
        }
    }
}