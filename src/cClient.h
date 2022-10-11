
#include <string>
#include "await.h"
#include "cTCPex.h"

/// @brief TCP client using cTCPex

class cClient
{
public:

    void parse_command_line_options(int argc, char *argv[]);

    void connect_to_server();

    void connect_to_server(
        const std::string &serverAddress,
        const std::string &serverPort);

    void send(
        const std::string& msg );

    void run();

private:
    static std::string myString;
    std::string myServerAddress;
    std::string myServerPort;
    raven::set::cTCPex tcpex;
    raven::await::cAwait waiter;

    void key();
    void inputHandler();
    std::string msgProcessor(
        int client,
        const std::string &msg);

};