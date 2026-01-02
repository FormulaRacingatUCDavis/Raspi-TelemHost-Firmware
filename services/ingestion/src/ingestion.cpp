#include "ingestion.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can/raw.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <ios>

namespace telem
{
    Bus::Bus(const char* interface) : s(socket(PF_CAN, SOCK_RAW, CAN_RAW))
    {
        strcpy(ifr.ifr_name, interface);
        if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
        {
            perror("ioctl SIOCGIFINDEX");
            exit(1);
        }

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        bind(s, (struct sockaddr *)&addr, sizeof(addr));
    }

    void Bus::add_frames(moodycamel::ConcurrentQueue<struct can_frame>& q)
    {
        while (1)
        {
            nbytes = read(s, &frame, sizeof(struct can_frame));
            if (nbytes < (int)sizeof(struct can_frame))
            {
                perror("CAN RAW socket recvmsg");
                exit(1);
            }

            q.enqueue(frame);
        }
    }

    void publish_can(Bus& pcan, Bus& tcan, struct mosquitto* mosq)
    {
        moodycamel::ConcurrentQueue<struct can_frame> q(8192);
        std::thread pcan_thread([&]{ pcan.add_frames(q); });
        std::thread tcan_thread([&]{ tcan.add_frames(q); });

        auto start = std::chrono::system_clock::now();
        std::ofstream log("test.csv");

        struct can_frame frame;
        uint32_t id;
        char topic[32];

        while (1) {
            while (q.try_dequeue(frame))
            {
                id = (frame.can_id & CAN_EFF_FLAG) ? (frame.can_id & CAN_EFF_MASK) : (frame.can_id & CAN_SFF_MASK);
                auto now = std::chrono::system_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

                snprintf(topic, sizeof(topic), "vehicle/can/%X", id);
                mosquitto_publish(mosq, nullptr, topic, sizeof(frame), &frame, 0, false);

                log << std::hex << std::uppercase
                    << id << ","
                    << static_cast<int>(frame.data[0]) << ","
                    << static_cast<int>(frame.data[1]) << ","
                    << static_cast<int>(frame.data[2]) << ","
                    << static_cast<int>(frame.data[3]) << ","
                    << static_cast<int>(frame.data[4]) << ","
                    << static_cast<int>(frame.data[5]) << ","
                    << static_cast<int>(frame.data[6]) << ","
                    << static_cast<int>(frame.data[7]) << ","
                    << std::dec << elapsed
                    << "\n";
                log.flush();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}