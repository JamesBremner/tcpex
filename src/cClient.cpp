#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "cCommandParser.h"
#include "await.h"
#include "cTCPex.h"
#include "cClient.h"

std::string cClient::myString;

cClient::cClient(int argc, char *argv[])
{
        parse_command_line_options(
        argc,
        argv);

    connect_to_server();

    run();
}

std::string cClient::msgProcessor(
    int client,
    const std::string &msg)
{
    std::cout << msg;
    return "";
}

void cClient::connect_to_server()
{
    auto pf = std::bind(
        &msgProcessor, this,
        std::placeholders::_1,
        std::placeholders::_2);

    while (1)
    {
        if (tcpex.connect_to_server(
                myServerAddress,
                myServerPort,
                pf))
            break;
        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }
    std::cout << "connected to server\n";
}

void cClient::key()
{
    char name[1024];
    std::cout << "type something: ";
    std::cin.getline(name, 1024);
    myString = name + std::string("\n");
}

void cClient::inputHandler()
{
    // user has typed something - send it to the server
    tcpex.send(myString);

    // std::cout << theCIN.myString;

    // wait for more typing
    waiter(
        std::bind(
            &key, this),
        std::bind(
            &inputHandler, this));
}

void cClient::parse_command_line_options(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("a", "address of server");
    P.add("p", "port where server listens for clients");

    P.parse(argc, argv);

    myServerAddress = P.value("a");
    myServerPort = P.value("p");

    if (myServerAddress.empty() || myServerPort.empty())
    {
        P.describe();
        exit(1);
    }
}
void cClient::run()
{
    waiter(
        std::bind(
            &key, this),
        std::bind(
            &inputHandler, this));
    waiter.run();
}
