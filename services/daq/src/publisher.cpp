#include <iostream>
#include <fstream>
#include <mosquitto.h>
#include <vector>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

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

    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr);
    mosquitto_connect(mosq, "localhost", 1883, 60);

    std::vector<std::thread> threads;
    auto q = telem::queue_can({pcan, tcan}, threads);

    telem::Capture cap;
    while (true)
    {
        if (q.try_dequeue(cap))
        {
            auto j = telem::decode_to_json(cap);
            if (!j.empty())
            {
                uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG) ? (cap.frame.can_id & CAN_EFF_MASK) : (cap.frame.can_id & CAN_SFF_MASK);

                char topic[64];
                snprintf(topic, sizeof(topic), "vehicle/can/%X", id);

                std::string payload = j.dump();
                mosquitto_publish(mosq, nullptr, topic, payload.size(), payload.c_str(), 0, false);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    return 0;
}