#include <thread>
#include <iostream>
#include <moodycamel/concurrentqueue.h>
#include <mosquitto.h>
#include "ingestion.h"

int main()
{
    /* CAN bus data ingestion */
    moodycamel::ConcurrentQueue<telem::Capture> q(8192);

    telem::Bus pcan("vcan0");
    telem::Bus tcan("vcan1");

    std::thread pcan_thread([&] {pcan.add_frames(q);});
    std::thread tcan_thread([&] {tcan.add_frames(q);});

    /* Mosquitto broker */
    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr);
    mosquitto_connect(mosq, "localhost", 1883, 60);

    telem::Capture cap{};
    while (1)
    {
        while (q.try_dequeue(cap))
        {
            char payload[128];

            uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG)
                ? (cap.frame.can_id & CAN_EFF_MASK)
                : (cap.frame.can_id & CAN_SFF_MASK);

            snprintf(payload, sizeof(payload), "%X %ld.%09ld",
                    id, cap.timestamp.tv_sec, cap.timestamp.tv_nsec);

            // Publish to topic "vehicle/can"
            int ret = mosquitto_publish(mosq, nullptr, "vehicle/can", std::strlen(payload), payload, 0, false);

            if (ret != MOSQ_ERR_SUCCESS)
                std::cerr << "MQTT publish error: " << mosquitto_strerror(ret) << "\n";

            // std::cout << std::hex << id
            //           << std::dec << " "
            //           << cap.timestamp.tv_sec << "."
            //           << cap.timestamp.tv_nsec
            //           << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}