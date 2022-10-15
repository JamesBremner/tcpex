#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cTCPex.h"
#include "cCommandParser.h"

struct sConfig
{
    std::string bbfile;
    std::string serverPort;
};

class cMessage
{
    int id;

    std::string line;

public:
    cMessage(const std::string &line);

    /// @brief read last message ID from bbfile, so new msg gets unique ID
    static void setLastMsgID();
    int write();

    std::string sender;
    static int myLastID;
};

/// @brief The server
class cServer
{
public:
    cServer();

    void startServer();

private:
    raven::set::cTCPex myTCPServer;
    std::map<std::string, std::string> myMapUser;

    enum class eCommand
    {
        none,
        user,
        read,
        write,
        replace,
        quit,
    };

    enum class eReturn
    {
        OK,
        read_error,
        unknown,
    };

    eCommand myLastCommand;
    int myLastID;
    std::string myLastReplaced;

    std::string eventHandler(
        const raven::set::cTCPex::sEvent& e);

    /// @brief check that all characters in message are legal
    /// @param msg
    /// @return true if all OK
    bool checkLegal(
        const std::string &msg) const;

    /// @brief Which command has been received
    /// @param[in] msg  input message
    /// @param[out] cmdtext text of command
    /// @return the command
    eCommand parseCommand(
        const std::string &msg,
        std::string &cmdtext) const;

    /// @brief find message in bbfile
    /// @param[in] number  ID of message
    /// @param[out] response to send to client
    /// @return status
    eReturn msgFind(
        const std::string &number,
        std::string &response);

    eReturn replace(
        const std::string &port,
        const std::string &cmdtext);

    eReturn erase(int number);

    void rollback();
};

sConfig theConfig;
cServer theServer;
int cMessage::myLastID = -1;

cMessage::cMessage(const std::string &m)
    : sender("nobody"), line(m)
{
}
int cMessage::write()
{
    id = ++myLastID;
    std::ofstream ofs(
        theConfig.bbfile,
        std::ios::app);
    if (!ofs.is_open())
        return -1;
    ofs << id << "/" << sender << "/" << line;
    std::cout << "wrote " << id << "\n";
    return id;
}

void commandParser(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("b", "the file name bbfile");
    P.add("p", "port where server listens for clients");
    P.add("c", "configuration file");

    P.parse(argc, argv);

    theConfig.bbfile = P.value("b");
    if (theConfig.bbfile.empty())
        throw std::runtime_error(
            " bbfile name not specified");

    theConfig.serverPort = P.value("p");
    if (theConfig.serverPort.empty())
        theConfig.serverPort = "9000";
}

void cMessage::setLastMsgID()
{
    std::ifstream ifs(
        theConfig.bbfile);
    if (!ifs.is_open())
        return;
    std::string line;
    while (getline(ifs, line))
    {
        if (cMessage::myLastID < atoi(line.c_str()))
            cMessage::myLastID = atoi(line.c_str());
    }
}

cServer::cServer()
    : myLastCommand(eCommand::none)
{
}

void cServer::startServer()
{
    cMessage::setLastMsgID();

    // wait for connection request
    try
    {
        myTCPServer.lineAccumulator(true);
        myTCPServer.sharedProcessingThread(true);

        myTCPServer.start_server(
            theConfig.serverPort,
            std::bind(
                &eventHandler, this,
                std::placeholders::_1),
            2);

        std::cout << "Waiting for connection on port "
                  << theConfig.serverPort << "\n";

        /* The server is running a thread listening for client requests
            Let it get on with things
        */
        while (1)
            std::this_thread::sleep_for(
                std::chrono::seconds(1));
    }
    catch (std::runtime_error &e)
    {
        throw std::runtime_error(
            "Cannot start server " + std::string(e.what()));
    }
}
std::string cServer::eventHandler(
    const raven::set::cTCPex::sEvent& e)
{
    switch (e.type)
    {
    case raven::set::cTCPex::eEvent::accept:
        myTCPServer.send("0.0 greeting\n", e.client);
        return "";
    case raven::set::cTCPex::eEvent::disconnect:
        std::cout << "disconnect event client " << e.client << "\n";
        return "";
    case raven::set::cTCPex::eEvent::read:
    {
        if (!checkLegal(e.msg))
            return "ERROR illegal character";

        std::string cmdtext;
        switch (parseCommand(e.msg, cmdtext))
        {
        case eCommand::write:
        {
            cMessage M(cmdtext);
            auto it = myMapUser.find(std::to_string(e.client));
            if (it == myMapUser.end())
                M.sender = "nobody";
            else
                M.sender = it->second;
            myLastID = M.write();
            return "3.0 WROTE " + std::to_string(myLastID) + "\n";
        }

        case eCommand::user:
            cmdtext.erase(cmdtext.find("\n"));
            myMapUser.insert(
                std::make_pair(
                    std::to_string(e.client),
                    cmdtext));
            return "1.0 HELLO " + cmdtext + "\n";

        case eCommand::read:
        {
            std::string response;
            msgFind(
                cmdtext,
                response);
            return response;
        }

        case eCommand::replace:
            replace(
                std::to_string(e.client),
                cmdtext);
            return "";

        default:
            return "unrecognized command";
        }
    }
    break;
    }
    return "";
}

bool cServer::checkLegal(
    const std::string &msg) const
{
    for (char c : msg)
    {
        if (c == '/')
            return false;
    }
    return true;
}

cServer::eCommand cServer::parseCommand(
    const std::string &msg,
    std::string &cmdtext) const
{
    auto scmd = msg.substr(0, 4);
    if (scmd == "WRIT")
    {
        cmdtext = msg.substr(6);
        return eCommand::write;
    }
    else if (scmd == "USER")
    {
        cmdtext = msg.substr(5);
        return eCommand::user;
    }
    else if (scmd == "READ")
    {
        cmdtext = msg.substr(5);
        return eCommand::read;
    }
    else if (scmd == "REPL")
    {
        cmdtext = msg.substr(8);
        return eCommand::replace;
    }
    else
        return eCommand::none;
}

cServer::eReturn cServer::msgFind(
    const std::string &number,
    std::string &response)
{
    int mid = atoi(number.c_str());
    std::ifstream ifs(
        theConfig.bbfile);
    if (!ifs.is_open())
    {
        response = "2.2 ERROR READ text\n";
        return eReturn::read_error;
    }
    std::string line;
    while (getline(ifs, line))
    {
        if (atoi(line.c_str()) == mid)
        {
            line[line.find("/")] = ' ';
            response = "2.0 MESSAGE " + line + std::string("\n");
            return eReturn::OK;
        }
    }
    response = "2.1 UNKNOWN " + number + "\n";
    return eReturn::unknown;
}

cServer::eReturn cServer::replace(
    const std::string &port,
    const std::string &cmdtext)
{
    int p = cmdtext.find("/");
    if (p == -1)
        return eReturn::unknown;
    auto number = cmdtext.substr(0, p);
    auto msg = cmdtext.substr(p + 1);
    std::vector<std::string> vbb;
    std::ifstream ifs(theConfig.bbfile);
    if (!ifs.is_open())
    {
        myTCPServer.send(
            "3.2 ERROR WRITE " + number + "\n");
        return eReturn::read_error;
    }
    std::string line;
    bool found = false;
    while (getline(ifs, line))
    {
        p = line.find("/");
        if (number == line.substr(0, p))
        {
            // found matching message number
            myLastReplaced = line;
            std::string sender("nobody");
            auto it = myMapUser.find(port);
            if (it != myMapUser.end())
                sender = it->second;
            line = number + "/" + sender + "/" + msg;
            found = true;
        }
        vbb.push_back(line);
    }
    ifs.close();
    if (!found)
    {
        myTCPServer.send(
            "3.1 UNKNOWN " + number + "\n");
        return eReturn::unknown;
    }

    std::ofstream ofs(theConfig.bbfile);
    if (!ofs.is_open())
        return eReturn::read_error;
    for (auto &l : vbb)
        ofs << l + "\n";
    myTCPServer.send(
        "3.0 WROTE " + number + "\n");
    myLastCommand = eCommand::replace;
    myLastID = atoi(number.c_str());
    return eReturn::OK;
}

cServer::eReturn cServer::erase(int number)
{
    auto snumber = std::to_string(number);
    std::vector<std::string> vbb;
    std::ifstream ifs(theConfig.bbfile);
    if (!ifs.is_open())
    {
        myTCPServer.send(
            "3.2 ERROR WRITE " + std::to_string(number) + "\n");
        return eReturn::read_error;
    }
    std::string line;
    while (getline(ifs, line))
    {
        if (snumber != line.substr(0, line.find("/")))
            vbb.push_back(line);
    }
    ifs.close();
    std::ofstream ofs(theConfig.bbfile);
    if (!ofs.is_open())
        return eReturn::read_error;
    for (auto &l : vbb)
        ofs << l + "\n";

    return eReturn::OK;
}
void cServer::rollback()
{
    switch (myLastCommand)
    {
    case eCommand::write:
        erase(myLastID);
        break;
    case eCommand::replace:
        erase(myLastID);

        break;
    default:
        break;
    }
}

main(int argc, char *argv[])
{
    try
    {
        commandParser(argc, argv);
        theServer.startServer();
    }
    catch (std::runtime_error &e)
    {
        std::cout << "runtime error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
