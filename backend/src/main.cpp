#include "ingestion.h"
#include <thread>

int main()
{
    frucd::Bus pcan("vcan0");
    frucd::Capture pcan_cap;

    while (1)
    {
        pcan_cap = pcan.get_frame();
        uint32_t id = (pcan_cap.frame.can_id & CAN_EFF_FLAG) ? (pcan_cap.frame.can_id & CAN_EFF_MASK) : (pcan_cap.frame.can_id & CAN_SFF_MASK);

        std::cout << std::hex << id;
        std::cout << std::dec << " " << pcan_cap.timestamp.tv_sec << "." << pcan_cap.timestamp.tv_nsec << std::endl;
    }

    return 0;
}