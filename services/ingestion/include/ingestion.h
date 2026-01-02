#pragma once

#include <cstdint>
#include <thread>
#include <moodycamel/concurrentqueue.h>
#include <mosquitto.h>
#include <linux/can.h>
#include <net/if.h>

namespace telem
{
    class Bus
    {
    public:
        Bus(const char* interface);
        void add_frames(moodycamel::ConcurrentQueue<struct can_frame>& q);

    private:
        int s;
        struct ifreq ifr;
        struct sockaddr_can addr;
        struct can_frame frame;
        int nbytes;
    };

    void publish_can(Bus& pcan, Bus& tcan, struct mosquitto* mosq);
}