#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <cctype>

class AEngine
{
public:
    virtual ~AEngine() = default;
    virtual void forward(int time_ms) = 0;
    virtual void stop() = 0;
    virtual void right(int time_ms) = 0;
    virtual void left(int time_ms) = 0;
};

class FooEngine : public AEngine
{
public:
    void forward(int time_ms) override
    {
        std::cout << "[Engine] Forward to " << time_ms << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    }

    void stop() override
    {
        std::cout << "[Engine] Stop" << std::endl;
    }

    void right(int time_ms) override
    {
        std::cout << "[Engine] Turn right to " << time_ms << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    }

    void left(int time_ms) override
    {
        std::cout << "[Engine] Turn left to " << time_ms << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    }
};

class ACmdReceiver
{
public:
    virtual ~ACmdReceiver() = default;
    virtual std::string receive() = 0;
};

class FooCmdReceiver : public ACmdReceiver
{
public:
    std::string receive() override
    {
        std::string cmd;
        std::cout << "Enter command (forward <ms>, stop, right <ms>, left <ms>, quit for exit): ";
        std::getline(std::cin, cmd);
        return cmd;
    }
};

std::string trim(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

bool processCommand(const std::string &cmd, AEngine &engine)
{
    std::string trimmed = trim(cmd);
    if (trimmed.empty())
        return true;

    std::istringstream iss(trimmed);
    std::string action;
    iss >> action;

    if (action == "quit")
    {
        return false;
    }
    else if (action == "stop")
    {
        engine.stop();
        return true;
    }
    else if (action == "forward" action == "right" action == "left")
    {
        int time_ms;
        if (iss >> time_ms)
        {
            if (action == "forward")
                engine.forward(time_ms);
            else if (action == "right")
                engine.right(time_ms);
            else if (action == "left")
                engine.left(time_ms);
        }
        else
        {
            std::cout << "Error: no time is specified for the team " << action << std::endl;
        }
        return true;
    }
    else
    {
        std::cout << "Unknown team. Try: forward <ms>, stop, right <ms>, left <ms>, quit" << std::endl;
        return true;
    }
}

int main()
{
    FooEngine engine;
    FooCmdReceiver receiver;

    std::cout << "Simulation of engine control. To exit, enter 'quit'." << std::endl;

    bool running = true;
    while (running)
    {
        std::string command = receiver.receive();
        running = processCommand(command, engine);
    }

    std::cout << "The program is completed." << std::endl;
    return 0;
}