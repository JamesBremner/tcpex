#include "cCommandParser.h"
#include "await.h"
#include "cTCPex.h"

std::string myServerAddress;
std::string myServerPort;
std::string myString;
raven::await::cAwait waiter;
raven::set::cTCPex tcpex;

void parse_command_line_options(int argc, char *argv[])
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
/// @brief blocking wait for complete line from keyboard
void key()
{
    char name[1024];
    std::cout << "type something: ";
    std::cin.getline(name, 1024);
    myString = name + std::string("\n");
}

/// @brief handle input from keyboard by sending to server, then waiting for another line
void inputHandler()
{
    // user has typed something - send it to the server
    tcpex.send(myString);

    // std::cout << theCIN.myString;

    // wait for more typing
    waiter(
        std::bind(
            &key),
        std::bind(
            &inputHandler));
}

/// @brief  get first input line from keyboard
void run()
{
    waiter(
        std::bind(
            &key),
        std::bind(
            &inputHandler));
    waiter.run();
}

std::string msgProcessor(
    int client,
    const std::string &msg)
{
    std::cout <<"\nfrom server: " <<msg << "\n";
    return "";
}

void connect_to_server()
{
    // loop until connection made
    while (1)
    {
        if (tcpex.connect_to_server(
                myServerAddress,
                myServerPort,
                std::bind(
                    &msgProcessor,
                    std::placeholders::_1,
                    std::placeholders::_2)))
            break;

        //connection failed, wait 1 second then try again
        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }
    std::cout << "connected to server\n";
}

main(int argc, char *argv[])
{

    parse_command_line_options(argc, argv);

    connect_to_server();

    run();

    return 0;
}
