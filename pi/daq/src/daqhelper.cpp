#include "daqhelper.h"

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
    DAQHelper::DAQHelper(const char* interface) : s(socket(PF_CAN, SOCK_RAW, CAN_RAW))
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

    DAQHelper::~DAQHelper()
    {
        close(s);
    }

    bool DAQHelper::read_frame(telem::Capture& cap)
    {
        int nbytes = read(s, &cap.frame, sizeof(struct can_frame));
        cap.timestamp = std::chrono::system_clock::now();
        return nbytes == sizeof(struct can_frame);
    }

    void DAQHelper::queue_frame(moodycamel::ConcurrentQueue<telem::Capture> &q)
    {
        telem::Capture frame;

        while (true)
        {
            if (read_frame(frame))
            {
                q.enqueue(frame);
            }
        }
    }
}