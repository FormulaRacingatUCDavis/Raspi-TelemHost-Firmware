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

        bind(s, (struct sockaddr *)&addr, sizeof(addr));
    }

    void Bus::add_frames(moodycamel::ConcurrentQueue<Capture>& q)
    {
        Capture cap;

        while (1)
        {
            nbytes = read(s, &cap.frame, sizeof(struct can_frame));
            cap.timestamp = std::chrono::system_clock::now();
            if (nbytes < (int)sizeof(struct can_frame))
            {
                perror("CAN RAW socket recvmsg");
                exit(1);
            }

            q.enqueue(cap);
        }
    }

    moodycamel::ConcurrentQueue<telem::Capture> queue_can(const std::vector<std::string>& ifaces, std::vector<std::thread>& threads)
    {
        moodycamel::ConcurrentQueue<telem::Capture> q(8192);

        for (const auto& iface : ifaces)
        {
            auto bus = new telem::Bus(iface.c_str());
            threads.emplace_back([bus, &q] {
                bus->add_frames(q);
                delete bus;
            });
        }

        return q;
    }
}