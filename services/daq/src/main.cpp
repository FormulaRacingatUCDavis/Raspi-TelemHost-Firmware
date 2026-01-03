#include <mosquitto.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <atomic>

#include "ingestion.h"
#include "decode.h"

std::atomic<bool> logging_enabled{false};

void on_message(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* msg);

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

    pid_t pid = fork();
    if (pid == 0)
    {
        /* Child process: Log to CSV and listen to MQTT for logging control */

        mosquitto_lib_init();
        struct mosquitto* mosq = mosquitto_new("can-logger", true, nullptr);
        mosquitto_connect(mosq, "localhost", 1883, 60);

        mosquitto_message_callback_set(mosq, on_message);
        mosquitto_subscribe(mosq, nullptr, "vehicle/logger/status", 1);
        mosquitto_loop_start(mosq);

        std::filesystem::path log_dir;
        if (pcan.rfind("vcan", 0) == 0 || tcan.rfind("vcan", 0) == 0)
            log_dir = "testlogs";
        else
            log_dir = "logs";
        std::filesystem::create_directories(log_dir);

        std::ofstream log;
        std::filesystem::path log_file;
        auto start = std::chrono::system_clock::now();
        
        std::string payload = "paused";
        bool was_logging = false;
        mosquitto_publish(mosq, nullptr, "vehicle/logger/status", payload.size(), payload.c_str(), 0, true);

        std::vector<std::thread> threads;
        auto q = telem::queue_can({pcan, tcan}, threads, &logging_enabled);

        telem::Capture cap;
        while (true)
        {
            bool active = logging_enabled.load();

            if (active && !was_logging)
            {
                start = std::chrono::system_clock::now();
                log_file = log_dir / (telem::format_timestamp(start) + ".csv");
                log.close();
                log.open(log_file, std::ios::out | std::ios::app);

                while (q.try_dequeue(cap)) {}

                std::cout << "[LOGGER] Started new log: " << log_file << "\n";
            }

            was_logging = active;

            if (!active)
            {
                while (q.try_dequeue(cap)) {}
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            if (q.try_dequeue(cap))
            {
                uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG) ? (cap.frame.can_id & CAN_EFF_MASK)
                                                                : (cap.frame.can_id & CAN_SFF_MASK);
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

        _exit(0);
    }
    else if (pid > 0)
    {
        /* Publish CAN bus data to MQTT broker */

        std::cout << "Forked CAN logger process, pid = " << pid << std::endl;

        mosquitto_lib_init();
        struct mosquitto* mosq = mosquitto_new("can-publisher", true, nullptr);
        mosquitto_connect(mosq, "localhost", 1883, 60);
        mosquitto_loop_start(mosq);

        std::vector<std::thread> threads;
        auto q = telem::queue_can({pcan, tcan}, threads, nullptr);

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

        _exit(0);
    }

    return 0;
}

void on_message(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* msg)
{
    if (!msg || !msg->payload)
        return;

    std::string topic(msg->topic);
    std::string payload(static_cast<char*>(msg->payload), msg->payloadlen);

    if (topic == "vehicle/logger/status")
    {
        if (payload == "running")
            logging_enabled.store(true);
        else if (payload == "paused")
            logging_enabled.store(false);
    }
}