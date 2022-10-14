#include <thread>
#include <chrono>

#include "cCommandParser.h"
#include "cTCPex.h"

std::string myServerPort1;
std::string myServerPort2;
raven::set::cTCPex tcpex1;
raven::set::cTCPex tcpex2;

void parse_command_line_options(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("p1", "port where server 1 listens for clients");
    P.add("p2", "port where server 2 listens for clients");

    P.parse(argc, argv);

    myServerPort1 = P.value("p1");
    myServerPort2 = P.value("p2");

    if (myServerPort1.empty() || myServerPort2.empty())
    {
        P.describe();
        exit(1);
    }
}

std::string msgHandler1(
    int client,
    raven::set::cTCPex::eEvent type,
    const std::string &msg)
{
    switch (type)
    {
    case raven::set::cTCPex::eEvent::accept:
        std::cout << client << " client connected\n";
        return "";

        std::cout << "Copy from 1 " << msg << "\n";
        tcpex2.send("REPLY: " + msg);
        return msg;
    default:
        return "";
    }
}
std::string msgHandler2(
    int client,
    raven::set::cTCPex::eEvent type,
    const std::string &msg)
{
    switch (type)
    {
    case raven::set::cTCPex::eEvent::accept:
        std::cout << client << " client connected\n";
        return "";
    case raven::set::cTCPex::eEvent::read:
        std::cout << "Copy from 2: " << msg << "\n";
        tcpex1.send("REPLY: " + msg);
        return msg;
    default:
        return "";
    }
}

main(int argc, char *argv[])
{

    parse_command_line_options(argc, argv);

    tcpex1.start_server(
        myServerPort1,
        std::bind(
            &msgHandler1,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
    tcpex2.start_server(
        myServerPort2,
        std::bind(
            &msgHandler2,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));

    while (1)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}