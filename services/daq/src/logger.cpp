#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

#include "ingestion.h"
#include "decode.h"

int main()
{
    nlohmann::json cfg;
    std::ifstream cfg_file("config/config.json");
    if (!cfg_file.is_open()) {
        std::cerr << "Failed to open config.json\n";
        return 1;
    }
    cfg_file >> cfg;
    cfg_file.close();

    std::string pcan = cfg["interfaces"]["pcan"];
    std::string tcan = cfg["interfaces"]["tcan"];
    
    auto start = std::chrono::system_clock::now();

    std::filesystem::path log_dir;

    if (pcan.rfind("vcan", 0) == 0 || tcan.rfind("vcan", 0) == 0)
        log_dir = "testlogs";
    else
        log_dir = "logs";

    std::filesystem::create_directories(log_dir);

    std::filesystem::path log_file = log_dir / (telem::format_timestamp(start) + ".csv");
    std::ofstream log(log_file, std::ios::out | std::ios::app);

    std::vector<std::thread> threads;
    auto q = telem::queue_can({pcan, tcan}, threads);

    telem::Capture cap;
    while (true)
    {
        if (q.try_dequeue(cap))
        {
            uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG) ? (cap.frame.can_id & CAN_EFF_MASK) : (cap.frame.can_id & CAN_SFF_MASK);
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(cap.timestamp - start).count();

            log << std::hex << std::uppercase
                << id << ","
                << static_cast<int>(cap.frame.data[0]) << ","
                << static_cast<int>(cap.frame.data[1]) << ","
                << static_cast<int>(cap.frame.data[2]) << ","
                << static_cast<int>(cap.frame.data[3]) << ","
                << static_cast<int>(cap.frame.data[4]) << ","
                << static_cast<int>(cap.frame.data[5]) << ","
                << static_cast<int>(cap.frame.data[6]) << ","
                << static_cast<int>(cap.frame.data[7]) << ","
                << std::dec << elapsed
                << "\n";
            log.flush();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    return 0;
}