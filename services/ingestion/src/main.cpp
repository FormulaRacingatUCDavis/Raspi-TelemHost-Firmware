#include <mosquitto.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include "ingestion.h"

int main()
{
    pid_t pid = fork();
    
    if (pid == 0)
    {
        telem::Bus pcan("vcan0");
        telem::Bus tcan("vcan1");

        mosquitto_lib_init();
        struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr);
        mosquitto_connect(mosq, "localhost", 1883, 60);

        telem::publish_can(pcan, tcan, mosq);
        
        _exit(0);
    } 
    else if (pid > 0)
    {
        std::cout << "CAN publisher started, pid = " << pid << "\n";

        std::cout << "Press Enter to exit parent and stop logging...\n";
        std::cin.get();

        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
    }

    return 0;
}