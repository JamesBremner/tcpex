#include <thread>
#include <chrono>

#include "cCommandParser.h"
#include "cTCPex.h"

std::string myServerPort;
raven::set::cTCPex tcpex;

void parse_command_line_options(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("p", "port where server listens for clients");

    P.parse(argc, argv);

    myServerPort = P.value("p");

    if (myServerPort.empty())
    {
        P.describe();
        exit(1);
    }
}

std::string eventHandler(
    int client,
    raven::set::cTCPex::eEvent type,
    const std::string &msg)
{
    switch (type)
    {
    case raven::set::cTCPex::eEvent::accept:
        std::cout << "client connected\n";
        return "";
    case raven::set::cTCPex::eEvent::read:
        tcpex.send("REPLY: " + msg);
        return msg;
    }
    return "";
}

main(int argc, char *argv[])
{
    parse_command_line_options(argc, argv);

    tcpex.start_server(
        myServerPort,
        std::bind(
            &eventHandler,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));

    while (1)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}