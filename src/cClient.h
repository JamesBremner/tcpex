/// @brief TCP client using cTCPex
class cClient
{
public:
    /** @brief CTOR
    /// @param argc 
    /// @param argv 

    Start client, never returns
    */
    cClient(int argc, char *argv[]);

private:
    static std::string myString;
    std::string myServerAddress;
    std::string myServerPort;
    raven::set::cTCPex tcpex;
    raven::await::cAwait waiter;

    void parse_command_line_options(int argc, char *argv[]);
    void connect_to_server();
    void key();
    void inputHandler();
    std::string msgProcessor(
        int client,
        const std::string &msg);
    void run();
};