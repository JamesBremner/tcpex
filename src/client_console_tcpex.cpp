#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "cCommandParser.h"
#include "await.h"
#include "cTCPex.h"

class cCIN
{
public:
    static std::string myString;
    void operator()()
    {
        std::cout << "type something: ";
        std::cin >> myString;
        myString += "\n";
    }
};

std::string server_address;
std::string server_port;
raven::await::cAwait waiter;
raven::set::cTCPex tcpex;
std::string cCIN::myString;
cCIN theCIN;

void inputHandler()
{
    // user has typed something - send it to the server
    tcpex.send(theCIN.myString);

    // std::cout << theCIN.myString;

    // wait for more typing
    waiter(theCIN, inputHandler);
}

void parse_command_line_options(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("a", "address of server");
    P.add("p", "port where server listens for clients");

    P.parse(argc, argv);

    server_address = P.value("a");
    server_port = P.value("p");

    if (server_address.empty() || server_port.empty())
    {
        P.describe();
        exit(1);
    }
}

std::string msgProcessor(
    int client,
    const std::string &msg)
{
    std::cout << msg;
    return "";
}

main(int argc, char *argv[])
{
    parse_command_line_options(
        argc,
        argv);

    auto pf = std::bind(
        &msgProcessor,
        std::placeholders::_1,
        std::placeholders::_2);

    tcpex.connect_to_server(
        server_address,
        server_port,
        pf);

    // wait for user to type something
    waiter(
        theCIN,
        inputHandler);

    waiter.run();

    return 0;
}
