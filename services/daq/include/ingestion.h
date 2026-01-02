#pragma once

#include <cstdint>
#include <thread>
#include <moodycamel/concurrentqueue.h>
#include <linux/can.h>
#include <net/if.h>
#include <chrono>
#include <vector>

namespace telem
{
    struct Capture {
        struct can_frame frame;
        std::chrono::system_clock::time_point timestamp;
    };

    class Bus
    {
    public:
        Bus(const char* interface);
        void add_frames(moodycamel::ConcurrentQueue<Capture>& q);

    private:
        int s;
        struct ifreq ifr;
        struct sockaddr_can addr;
        int nbytes;
    };

    moodycamel::ConcurrentQueue<telem::Capture> queue_can(const std::vector<std::string>& ifaces, std::vector<std::thread>& threads);
}