#include <iostream>
#include <thread>
#include <chrono>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "robot_gpio.h"   // <-- подключаем заголовок GPIO

using json = nlohmann::json;

int main() {
    robot_gpio_init();   // инициализация (если требуется)

    httplib::Server svr;

    svr.Post("/commands", [](const httplib::Request& req, httplib::Response& res) {
        if (req.get_header_value("Content-Type") != "application/json") {
            res.status = 415;
            res.set_content("Unsupported Content-Type. Expected application/json", "text/plain");
            return;
        }

        try {
            json request_json = json::parse(req.body);
            std::string command = request_json.value("command", "");
            int duration = request_json.value("duration", 0);
            int id = request_json.value("id", 0);

            std::string action_message;
            bool valid = true;

            if (command == "forward") {
                action_message = "Moving forward for " + std::to_string(duration) + " ms";
                robot_forward();
                std::cout << "forward" << std::endl;
            } else if (command == "left") {
                action_message = "Turning left for " + std::to_string(duration) + " ms";
                robot_left();
                std::cout << "left" << std::endl;
            } else if (command == "right") {
                action_message = "Turning right for " + std::to_string(duration) + " ms";
                robot_right();
                std::cout << "right" << std::endl;
            } else if (command == "stop") {
                action_message = "Stopping motors";
                robot_stop();
                std::cout << "stop" << std::endl;
            } else {
                valid = false;
                res.status = 400;
                res.set_content("Unknown command: " + command, "text/plain");
            }

            if (valid) {
                if (duration > 0 && command != "stop") {
                    // Запускаем таймер на остановку через duration миллисекунд
                    std::thread([duration]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
                        robot_stop();
                        std::cout << "[ROBOT] Auto-stop after " << duration << " ms\n";
                    }).detach();
                }

                std::cout << "[ROBOT] " << action_message << " (id=" << id << ")" << std::endl;

                json response_json;
                response_json["message"] = action_message;
                response_json["received_id"] = id;
                response_json["status"] = "success";
                res.set_content(response_json.dump(), "application/json");
                res.status = 200;
            }

        } catch (const json::parse_error& e) {
            res.status = 400;
            res.set_content("Invalid JSON format: " + std::string(e.what()), "text/plain");
        }
    });

    std::cout << "Server listening on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    robot_gpio_cleanup();
    return 0;
}