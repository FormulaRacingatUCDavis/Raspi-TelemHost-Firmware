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
#include <thread>
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

        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind");
            exit(1);
        }
    }

    Bus::~Bus()
    {
        close(s);
    }

    bool Bus::read_frame(telem::Capture& cap)
    {
        int nbytes = read(s, &cap.frame, sizeof(struct can_frame));
        cap.timestamp = std::chrono::system_clock::now();
        return nbytes == sizeof(struct can_frame);
    }

    moodycamel::ConcurrentQueue<telem::Capture> queue_can(
        const std::vector<std::string>& ifaces,
        std::vector<std::thread>& threads,
        std::atomic<bool>* logging_enabled)
    {
        moodycamel::ConcurrentQueue<telem::Capture> q(8192);

        for (const auto& iface : ifaces)
        {
            auto bus = new telem::Bus(iface.c_str());

            threads.emplace_back([bus, &q, logging_enabled] {
                telem::Capture frame;
                while (true)
                {
                    if (bus->read_frame(frame))
                    {
                        if (!logging_enabled || logging_enabled->load())
                            q.enqueue(frame);
                        else
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                    else
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }
                delete bus;
            });
        }

        return q;
    }
}